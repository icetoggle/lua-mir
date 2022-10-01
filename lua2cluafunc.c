#include "lua2cluafunc.h"
#include <lstate.h>
#include <lgc.h>
#include <lapi.h>
#include <stdbool.h>
#include "membuf.h"
#include <lopcodes.h>
#include "c2cluafunc.h"
#include "parse_opcode.h"
#include <assert.h>
#define ispseudo(i)		((i) <= LUA_REGISTRYINDEX)

static TValue *index2value (lua_State *L, int idx) {
  CallInfo *ci = L->ci;
  if (idx > 0) {
    StkId o = ci->func + idx;
    api_check(L, idx <= L->ci->top - (ci->func + 1), "unacceptable index");
    if (o >= L->top) return &G(L)->nilvalue;
    else return s2v(o);
  }
  else if (!ispseudo(idx)) {  /* negative index */
    api_check(L, idx != 0 && -idx <= L->top - (ci->func + 1), "invalid index");
    return s2v(L->top + idx);
  }
  else if (idx == LUA_REGISTRYINDEX)
    return &G(L)->l_registry;
  else {  /* upvalues */
    idx = LUA_REGISTRYINDEX - idx;
    api_check(L, idx <= MAXUPVAL + 1, "upvalue index too large");
    if (ttisCclosure(s2v(ci->func))) {  /* C closure? */
      CClosure *func = clCvalue(s2v(ci->func));
      return (idx <= func->nupvalues) ? &func->upvalue[idx-1]
                                      : &G(L)->nilvalue;
    }
    else {  /* light C function or Lua function (through a hook)?) */
      api_check(L, ttislcf(s2v(ci->func)), "caller not a C function");
      return &G(L)->nilvalue;  /* no upvalues */
    }
  }
}


LClosure *lua_tolclosure(lua_State *L, int idx)
{
    TValue *o = index2value(L, idx);
    if(!ttisLclosure(o)) {
        return NULL;
    }
    LClosure *c = gco2lcl(val_(o).gc);
    return c;
}

LClosure *lua_pushlclosure(lua_State *L, LClosure *cl)
{
    setclLvalue2s(L, L->top, cl);
    api_incr_top(L);
    luaC_checkGC(L);
    return cl;
}


struct function {
    struct lua_State *L;
    struct Proto *p;
    char fname[32];
};

static bool jit_cancompile(Proto* f)
{
    return true;
}

static int get_jit_funcid(lua_State *L)
{
    lua_pushstring(L, "__jit_funcid");
    lua_gettable(L, LUA_REGISTRYINDEX);
    if(lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_pushstring(L, "__jit_funcid");
        lua_pushinteger(L, 0);
        lua_settable(L, LUA_REGISTRYINDEX);
        return 0;
    }
    int fun_id = lua_tointeger(L, -1);
    fun_id++;
    lua_pop(L, 1);
    lua_pushstring(L, "__jit_funcid");
    lua_pushinteger(L, fun_id);
    lua_settable(L, LUA_REGISTRYINDEX);
    return fun_id;
}





#define RA(i)	(base+GETARG_A(i))
#define RB(i)	(base+GETARG_B(i))
#define vRB(i)	s2v(RB(i))
#define KB(i)	(k+GETARG_B(i))
#define RC(i)	(base+GETARG_C(i))
#define vRC(i)	s2v(RC(i))
#define KC(i)	(k+GETARG_C(i))
#define RKC(i)	((TESTARG_k(i)) ? k + GETARG_C(i) : s2v(base + GETARG_C(i)))



bool codegen_lua2c(lua_State *L, LClosure *cl, int func_id, Membuf *buf)
{
    MCF("#include <lua.h>\n");
    MCF("#include <lauxlib.h>\n");
    MCF("#include <lualib.h>\n");
    MCF("#include <lobject.h>\n");
    MCF("#include <lstate.h>\n");
    MCF("#include <lgc.h>\n");
    MCF("#include <luaconf.h>\n");
    MCF("#include <lapi.h>\n");
    MCF("#include <llimits.h>\n");
    MCF("#include <lvm.h>\n");
    MCF("#define savestate(L,ci)		(L->top = ci->top)\n");
    MCF("#define Protect(exp)  (savestate(L,ci), (exp))\n");
    MCF("static int __jit_lfunc%d(lua_State *L) {\n", func_id);
    MCF("    CallInfo *ci = L->ci;\n");
    MCF("    StkId base = ci->func + 1;\n");
    for(int pc = 0; pc < cl->p->sizecode; ++pc)
    {
        const Instruction i = cl->p->code[pc];
        OpCode op = GET_OPCODE(i);
        int A = GETARG_A(i);
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        MCF("__jitfunc%d_op%d: \n", func_id, pc);
        switch(op) {
            case OP_ADD: {
                parse_op_arith(func_id, pc, buf, '+', A, B, C);
                break;
            }
            case OP_SUB: {
                parse_op_arith(func_id, pc, buf, '-', A, B, C);
                break;
            }
            case OP_MUL: {
                parse_op_arith(func_id, pc, buf, '*', A, B, C);
                break;
            }
            case OP_MOD: {
                parse_op_arith(func_id, pc, buf, '%', A, B, C);
                break;
            }
            case OP_POW: {
                parse_op_arithf(func_id, pc, buf, '^', A, B, C);
                break;
            }
            case OP_DIV: {
                parse_op_arithf(func_id, pc, buf, '/', A, B, C);
                break;
            }
            case OP_IDIV: {
                parse_op_arith(func_id, pc, buf, '\\', A, B, C);
                break;
            }
            case OP_MMBIN: {
                Instruction pi = cl->p->code[pc - 1];
                MCF("{\n");
                MCF("TValue *rb = s2v(base + %u);\n", B);
                MCF("TMS tm = (TMS)%u;\n", C);
                MCF("int flip = %u;\n", GETARG_k(i));
                MCF("StkId result = base + %u;\n", GETARG_A(pi));
                assert(OP_ADD <= GET_OPCODE(pi) && GET_OPCODE(pi) <= OP_SHR);
                MCF("luaT_trybinTM(L, s2v(base + %u), rb, result, tm);\n", A);
                MCF("}\n");
                break;
            }
            case OP_RETURN1: {
                MCF("{\n");
                MCF("setobjs2s(L, L->top, base + %u);\n", A);
                MCF("api_incr_top(L);\n");
                MCF("luaC_checkGC(L);\n");
                MCF("return 1;\n");
                MCF("}\n");
                break;
            }
            
            default:
                break;
        }
    }
    MCF("return 0;\n");
    MCF("}\n");
    return true;
}


int convert_lua_to_ccode(lua_State *L, LClosure *cl)
{
    if(!jit_cancompile(cl->p)) {
        return 0;
    }
    char fname[64];
    int fun_id = get_jit_funcid(L);
    sprintf(fname, "__jit_lfunc%d", fun_id);
    Membuf buff;
    membuf_init(&buff);
    if(!codegen_lua2c(L, cl, fun_id, &buff)) {
        membuf_free(&buff);
        return 0;
    }
    const char *code = membuf_to_string(&buff);
    lua_pushstring(L, code);
    membuf_free(&buff);
    return 1;
}


lua_CFunction create_clua_func_from_lua(lua_State *L, LClosure *cl)
{
    if(!jit_cancompile(cl->p)) {
        return NULL;
    }
    char fname[64];
    int fun_id = get_jit_funcid(L);
    sprintf(fname, "__jit_lfunc%d", fun_id);
    Membuf buff;
    membuf_init(&buff);
    if(!codegen_lua2c(L, cl, fun_id, &buff)) {
        membuf_free(&buff);
        return NULL;
    }
    const char *code = membuf_to_string(&buff);
    // printf("%s\n", code);
    return create_clua_func_from_c(L, fname, code);
    

}