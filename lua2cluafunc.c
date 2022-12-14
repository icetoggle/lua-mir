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
#include "testfunc.h"
#include "lua_std_cfunc.h"
#include <lopnames.h>
#include <ldebug.h>
#include "luamir_ctx.h"
#include <lauxlib.h>
#include <lobject.h>
#include <ltable.h>
#include <lvm.h>
#include <string.h>
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

CClosure *lua_tocclosure(lua_State *L, int idx) 
{
    const TValue *o = index2value(L, idx);
    if(!ttisCclosure(o)) {
        return NULL;
    }
    CClosure *c = gco2ccl(val_(o).gc);
    return c;
}


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


#define GEN_RKC(i) int testk = TESTARG_k(i);\
                if(testk) { \
                    MCF("TValue *rc = k + %d;\n", C); \
                } else { \
                    MCF("TValue *rc = s2v(base+%d);\n", C); \
                }

void opcode2c(lua_State* L, LuaMirContext *ctx,int func_id, Proto *p, bool scan_jump) 
{
    TValue *k = p->k;
    if(scan_jump) {
        ctx->is_scan_jump = true;
    }
    for(int pc = 0; pc < p->sizecode; ++pc)
    {
        const Instruction i = p->code[pc];
        OpCode op = GET_OPCODE(i);
        int A = GETARG_A(i);
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        if(array_exist(&ctx->exist_jumps, pc + 1))
        {
            MCF("__jitfunc%d_op%d: \n", func_id, pc + 1);
        }
        #ifdef JITDEBUG
        MCF("puts(\"//opcode is : %-9s %d\\n\");\n", opnames[op], luaG_getfuncline(p, pc));
        #endif
        MCF("//opcode is : %-9s %d\n", opnames[op], luaG_getfuncline(p, pc));
        MCF("updatebase(ci);\n");
        switch(op) {
            case OP_MOVE: {
                MCF("{\n");
                MCF("setobjs2s(L, base + %d, base + %d);\n", A, B);
                MCF("}\n");
                break;
            }
            case OP_LOADI: {
                MCF("{\n");
                MCF("lua_Integer b = %d;\n", GETARG_sBx(i));
                MCF("setivalue(s2v(base+%d), b);\n", A);
                MCF("}\n");
                break;
            }
            case OP_LOADF: {
                MCF("{\n");
                MCF("int b = %d;\n", GETARG_sBx(i));
                MCF("setfltvalue(s2v(base+%d), cast_num(b));\n", A);
                MCF("}\n");
                break;
            }
            case OP_LOADK: {
                parse_op_loadk(ctx, A, GETARG_Bx(i), k);
                break;
            }
            case OP_LOADKX: {
                parse_op_loadk(ctx, A, GETARG_Ax(p->code[++pc]), k);
                break;
            }
            case OP_LOADFALSE: {
                MCF("setbfvalue(s2v(base+%d));\n", A);
                break;
            }
            case OP_LFALSESKIP: {
                MCF("setbfvalue(s2v(base+%d));\n", A);
                GEN_GOTO_OP(func_id, pc + 2);
                break;
            }
            case OP_LOADTRUE: {
                MCF("setbtvalue(s2v(base+%d));\n", A);
                break;
            }
            case OP_LOADNIL: {
                MCF("{\n");
                MCF("int ra = %d;\n", A);
                MCF("int b = %d;\n", B);
                MCF("do {\n");
                MCF("setnilvalue(s2v(base+ra++));\n" );
                MCF("} while(b--);\n");
                MCF("}\n");
                break;
            }
            case OP_GETUPVAL: {
                MCF("{\n");
                MCF("int b = %d;\n", B);
                MCF("setobj2s(L, base+%d,func->upvalue + b);\n", A);
                MCF("}\n");
                break;
            }
            case OP_SETUPVAL: {
                MCF("{\n");
                MCF("int b = %d;\n", B);
                MCF("setobj(L, func->upvalue + b, s2v(base+%d));\n", A);
                MCF("}\n");
                break;
            }
            case OP_GETTABUP: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("int b = %d;\n", B);
                MCF("int c = %d;\n", C);
                MCF("TValue *upval = func->upvalue + b;\n");
                MCF("TValue *rc = k+c;\n");
                MCF("TString *key = tsvalue(rc);\n");
                MCF("if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {\n");
                MCF("   setobj2s(L,  base+%d, slot);\n", A);
                MCF("} else {\n");
                MCF("   luaV_finishget(L, upval, rc,  base+%d, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_GETTABLE: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("TValue *rb = s2v(base+%d);\n", B);
                MCF("TValue *rc = s2v(base+%d);\n", C);
                MCF("lua_Unsigned n;\n");
                MCF("if (ttisinteger(rc)  /* fast track for integers? */\n");
                MCF("    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))\n");
                MCF("    : luaV_fastget(L, rb, rc, slot, luaH_get)) {\n");
                MCF("   setobj2s(L, base + %d, slot);\n", A);
                MCF("} else {\n");
                MCF("   luaV_finishget(L, rb, rc,  base+%d, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_GETI: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("TValue *rb = s2v(base+%d);\n", B);
                MCF("if (luaV_fastgeti(L, rb, %d, slot)) {\n", C);
                MCF("   setobj2s(L, base+%d, slot);\n", A);
                MCF("} else {\n");
                MCF("   TValue key;\n");
                MCF("   setivalue(&key, %d);\n", C);
                MCF("   luaV_finishget(L, rb, &key,  base+%d, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_GETFIELD: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("TValue *rb = s2v(base+%d);\n", B);
                MCF("TValue *rc = k+%d;\n", C);
                MCF("TString *key =tsvalue(rc);\n");
                MCF("if (luaV_fastget(L, rb, key, slot, luaH_getshortstr)) {\n");
                MCF("   setobj2s(L, base+%d, slot);\n", A);
                MCF("} else {\n");
                MCF("   luaV_finishget(L, rb, rc,  base+%d, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_SETTABUP: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("TValue *upval = func->upvalue + %d;", A);
                MCF("TValue *rb = k + %d;\n", B);
                MCF("TValue *rc = k + %d;\n", C);
                MCF("TString *key = tsvalue(rb);\n");
                MCF("if (luaV_fastset(L, upval, key, slot, luaH_getshortstr)) {\n");
                MCF("   luaV_finishfastset(L, upval, slot, rc);\n");
                MCF("} else {\n");
                MCF("   luaV_finishset(L, upval, rb, rc, slot);\n");
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_SETTABLE: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("TValue *rb = s2v(base + %d); \n", B);
                GEN_RKC(i)
                MCF("lua_Unsigned n;\n");
                MCF("if (ttisinteger(rb)  /* fast track for integers? */\n");
                MCF("    ? (cast_void(n = ivalue(rb)), luaV_fastgeti(L, s2v(base + %d), n, slot))\n", A);
                MCF("    : luaV_fastget(L, s2v(base + %d), rb, slot, luaH_get)) {\n", A);
                MCF("   luaV_finishfastset(L, s2v(base + %d), slot, rc);\n", A);
                MCF("} else {\n");
                MCF("   luaV_finishset(L, s2v(base + %d), rb, rc, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_SETI: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("int c = %d;\n", B);
                GEN_RKC(i)
                MCF("if (luaV_fastgeti(L, s2v(base+%d), c, slot)) {\n", A);
                MCF("   luaV_finishfastset(L, s2v(base+%d), slot, rc);\n", A);
                MCF("} else {\n");
                MCF("   TValue key;\n");
                MCF("   setivalue(&key, c);\n");
                MCF("   luaV_finishset(L, s2v(base+%d), &key, rc, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_SETFIELD: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("TValue *rb = k + %d;\n", B);
                GEN_RKC(i)
                MCF("TString *key = tsvalue(rb);\n");
                MCF("if (luaV_fastget(L, s2v(base+%d), key, slot, luaH_getshortstr)) {\n", A);
                MCF("   luaV_finishfastset(L, s2v(base+%d), slot, rc);\n", A);
                MCF("} else {\n");
                MCF("   luaV_finishset(L, s2v(base+%d), rb, rc, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_NEWTABLE: {
                MCF("{\n");
                MCF("int b = %d;\n", B);
                MCF("int c = %d;\n", C);
                MCF("Table *t;\n");
                MCF("if (b > 0)\n");
                MCF("  b = 1 << (b - 1);  /* size is 2^(b - 1) */\n");
                if(TESTARG_k(i))
                    MCF("c += %d * (%d + 1);\n", GETARG_Ax(p->code[i+1]), MAXARG_C);
                MCF("L->top = base + %d + 1;    /* correct top in case of emergency GC */\n", A);
                MCF("t = luaH_new(L);  /* memory allocation */\n");
                MCF("sethvalue2s(L, base + %d, t);\n", A);
                MCF("if (b != 0 || c != 0)\n");
                MCF("  luaH_resize(L, t, c, b);  /* idem */\n");
                MCF("luaC_checkGC(L);\n");
                GEN_GOTO_OP(func_id, pc + 2);
                ++pc;
                MCF("}\n");
                break;
            }
            case OP_SELF: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("TValue *rb = s2v(base+%d);\n", B);
                GEN_RKC(i)
                MCF("TString *key = tsvalue(rc);\n");
                MCF("setobj2s(L, base + %d + 1, rb);\n", A);
                MCF("if (luaV_fastget(L, rb, key, slot, luaH_getstr)) {\n");
                MCF("   setobj2s(L, base+%d, slot);\n", A);
                MCF("} else {\n");
                MCF("   luaV_finishget(L, rb, rc, base+%d, slot);\n", A);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_ADDI: {
                parse_op_arithI(func_id, pc, ctx, '+', A, B, GETARG_sC(i));
                break;
            }
            case OP_ADDK: {
                parse_op_arithK(func_id, pc, ctx, '+', A, B, C);
                break;
            }
            case OP_SUBK: {
                parse_op_arithK(func_id, pc, ctx, '-', A, B, C);
                break;
            }
            case OP_MULK: {
                parse_op_arithK(func_id, pc, ctx, '*', A, B, C);
                break;
            }
            case OP_MODK: {
                parse_op_arithK(func_id, pc, ctx, '%', A, B, C);
                break;
            }
            case OP_POWK: {
                parse_op_arithKf(func_id, pc, ctx, '^', A, B, C);
                break;
            }
            case OP_DIVK: {
                parse_op_arithKf(func_id, pc, ctx, '/', A, B, C);
                break;
            }
            case OP_IDIVK: {
                parse_op_arithK(func_id, pc, ctx, '\\', A, B, C);
                break;
            }
            case OP_BANDK: {
                parse_op_bitwiseK(func_id, pc, ctx, '&', A, B, C);
                break;
            }
            case OP_BORK: {
                parse_op_bitwiseK(func_id, pc, ctx, '|', A, B, C);
                break;
            }
            case OP_BXORK: {
                parse_op_bitwiseK(func_id, pc, ctx, '^', A, B, C);
                break;
            }
            case OP_SHRI: {
                parse_op_shiftI(func_id, pc, ctx, '>', A, B, GETARG_sC(i));
                break;
            }
            case OP_SHLI: {
                parse_op_shiftI(func_id, pc, ctx, '<', A, B, GETARG_sC(i));
                break;
            }
            case OP_ADD: {
                parse_op_arith(func_id, pc, ctx, '+', A, B, C);
                break;
            }
            case OP_SUB: {
                parse_op_arith(func_id, pc, ctx, '-', A, B, C);
                break;
            }
            case OP_MUL: {
                parse_op_arith(func_id, pc, ctx, '*', A, B, C);
                break;
            }
            case OP_MOD: {
                parse_op_arith(func_id, pc, ctx, '%', A, B, C);
                break;
            }
            case OP_POW: {
                parse_op_arithf(func_id, pc, ctx, '^', A, B, C);
                break;
            }
            case OP_DIV: {
                parse_op_arithf(func_id, pc, ctx, '/', A, B, C);
                break;
            }
            case OP_IDIV: {
                parse_op_arith(func_id, pc, ctx, '\\', A, B, C);
                break;
            }
            case OP_BAND: {
                parse_op_bitwise(func_id, pc, ctx, '&', A, B, C);
                break;
            }
            case OP_BOR: {
                parse_op_bitwise(func_id, pc, ctx, '|', A, B, C);
                break;
            }
            case OP_BXOR: {
                parse_op_bitwise(func_id, pc, ctx, '^', A, B, C);
                break;
            }
            case OP_SHR: {
                parse_op_bitwise(func_id, pc, ctx, '>', A, B, C);
                break;
            }
            case OP_SHL: {
                parse_op_bitwise(func_id, pc, ctx, '<', A, B, C);
                break;
            }

            case OP_MMBIN: {
                Instruction pi = p->code[pc - 1];
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
            case OP_MMBINI: {
                Instruction pi = p->code[pc - 1];
                MCF("{\n");
                MCF("int imm = %d;\n", GETARG_sB(i));
                MCF("TMS tm = (TMS)%u;\n", C);
                MCF("int flip = %u;\n", GETARG_k(i));
                MCF("StkId result = base + %u;\n", GETARG_A(pi));
                assert(OP_ADDI <= GET_OPCODE(pi) && GET_OPCODE(pi) <= OP_SHRI);
                MCF("luaT_trybiniTM(L, s2v(base + %u), imm, flip, result, tm);\n", A);
                MCF("}\n");
                break;
            }
            case OP_MMBINK: {
                Instruction pi = p->code[pc - 1];
                MCF("{\n");
                MCF("TValue *imm = k + %d;\n", B);
                MCF("TMS tm = (TMS)%d;\n", C);
                MCF("int flip = %d;\n", GETARG_k(i));
                MCF("StkId result = base + %d;\n", GETARG_A(pi));
                MCF("luaT_trybinassocTM(L, s2v(base + %d), imm, flip, result, tm);\n", A);
                MCF("}\n");
                break;
            }
            case OP_UNM: {
                MCF("{\n");
                MCF("TValue *rb = s2v(base + %u);\n", B);
                MCF("lua_Number nb;\n");
                MCF("if (ttisinteger(rb)) {\n");
                MCF("  lua_Integer ib = ivalue(rb);\n");
                MCF("  setivalue(s2v(base + %d), intop(-, 0, ib));\n", A);
                MCF("}\n");
                MCF("else if (tonumberns(rb, nb)) {");
                MCF("  setfltvalue(s2v(base + %d), luai_numunm(L, nb));", A);
                MCF("}");
                MCF("else");
                MCF("  luaT_trybinTM(L, rb, rb, base + %d, TM_UNM);", A);
                MCF("}\n");
                break;
            }
            case OP_NOT: {
                MCF("{\n");
                MCF("TValue *rb = s2v(base + %u);\n", B);
                MCF("if (l_isfalse(rb))\n");
                MCF("  setbtvalue(s2v(base + %d));\n", A);
                MCF("else");
                MCF("  setbfvalue(s2v(base + %d));\n", A);
                MCF("}\n");
                break;
            }
            case OP_LEN: {
                MCF("{\n");
                MCF("luaV_objlen(L, base + %d, s2v(base + %d));\n", A, B);
                MCF("}\n");
                break;
            }
            case OP_CONCAT: {
                MCF("{\n");
                MCF("int n = %d;", B);
                MCF("L->top = base + %d + n;\n", A);
                MCF("luaV_concat(L, n);\n");
                MCF("luaC_checkGC(L);\n");
                MCF("}\n");
                break;
            }
            case OP_CALL: {
                MCF("{\n");
                MCF("int b = %d;\n", B);
                MCF("int nresults = %d - 1;\n", C);
                MCF("if (b != 0) L->top = (base + %d) + b;\n", A);
                MCF("luaD_callnoyield(L, base + %d, nresults);\n", A);
                MCF("adjustresults(L, nresults);\n");
                //MCF("print_stack(L, base);\n");
                MCF("}\n");
                break;
            }
            case OP_TAILCALL: {
                MCF("{\n");
                MCF("int b = %d;\n", B);
                MCF("if (b != 0) L->top = (base + %d) + b;\n", A);
                MCF("luaD_callnoyield(L, base + %d, LUA_MULTRET);\n", A);
                MCF("}\n");
                break;
            }
            
            //Todo  test
            case OP_CLOSE: {
                MCF("luaF_close(L, base + %d, LUA_OK, 1);\n", A);
                break;
            }
            //Todo  test
            case OP_TBC: {
                MCF("luaF_newtbcupval(L, base + %d);\n", A);
                break;
            }
            case OP_JMP: {
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(i) + 1);
                break;
            }
            case OP_EQ: {
                MCF("{\n");
                MCF("int cond;\n");
                MCF("TValue *rb = s2v(base + %d);\n", B);
                MCF("cond = luaV_equalobj(L, s2v(base + %d), rb);\n", A);
                MCF("if (cond != %d) {\n", GETARG_k(i));
                GEN_GOTO_OP(func_id, pc + 2);
                MCF("} else {\n");
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(p->code[pc + 1]) + 2);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_LT: {
                parse_op_order(func_id, pc, ctx, A, B, GETARG_k(i), GETARG_sJ(p->code[pc + 1]), "l_lti", "LTnum", "lessthanothers");
                break;
            }
            case OP_LE: {
                parse_op_order(func_id, pc, ctx, A, B, GETARG_k(i), GETARG_sJ(p->code[pc + 1]), "l_lei", "LEnum", "lessequalothers");
                break;
            }
            case OP_EQK : {
                MCF("{\n");
                MCF("TValue *rb = k + %d;\n", B);
                MCF("int cond = luaV_rawequalobj(s2v(base + %d), rb);\n", A);
                MCF("if (cond != %d) {\n", GETARG_k(i));
                GEN_GOTO_OP(func_id, pc + 2);
                MCF("} else {\n");
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(p->code[pc + 1]) + 2);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_EQI: {
                MCF("{\n");
                MCF("int cond;\n");
                MCF("int im = %d;\n", GETARG_sB(i));
                MCF("if (ttisinteger(s2v(base + %d)))\n", A);
                MCF(" cond = (ivalue(s2v(base + %d)) == im);\n", A);
                MCF("else if (ttisfloat(s2v(base + %d)))\n", A);
                MCF(" cond = luai_numeq(fltvalue(s2v(base + %d)), cast_num(im));\n", A);
                MCF("else\n");
                MCF(" cond = 0;\n");
                MCF("if (cond != %d) {\n", GETARG_k(i));
                GEN_GOTO_OP(func_id, pc + 2);
                MCF("} else {\n");
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(p->code[pc + 1]) + 2);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_LTI: {
                parse_op_orderI(func_id, pc, ctx, A, B, C, GETARG_k(i), GETARG_sJ(p->code[pc + 1]), "l_lti", "luai_numlt", "0", "TM_LT");
                break;
            }
            case OP_LEI: {
                parse_op_orderI(func_id, pc, ctx, A, B, C, GETARG_k(i), GETARG_sJ(p->code[pc + 1]), "l_lei", "luai_numle", "0", "TM_LE");
                break;
            }
            case OP_GTI: {
                parse_op_orderI(func_id, pc, ctx, A, B, C, GETARG_k(i), GETARG_sJ(p->code[pc + 1]), "l_gti", "luai_numgt", "1", "TM_LT");
                break;
            }
            case OP_GEI: {
                parse_op_orderI(func_id, pc, ctx, A, B, C, GETARG_k(i), GETARG_sJ(p->code[pc + 1]), "l_gei", "luai_numge", "1", "TM_LE");
                break;
            }
            case OP_TEST: {
                MCF("{\n");
                MCF("int cond = !l_isfalse(s2v(base + %d));\n", A);
                MCF("if (cond != %d) {\n", GETARG_k(i));
                GEN_GOTO_OP(func_id, pc + 2);
                MCF("} else {\n");
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(p->code[pc + 1]) + 2);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_TESTSET: {
                MCF("{\n");
                MCF("TValue *rb = s2v(base + %d);\n", B);
                MCF("if (l_isfalse(rb) == %d)\n {", GETARG_k(i));
                GEN_GOTO_OP(func_id, pc + 2);
                MCF("} else {\n");
                MCF("  setobj2s(L, base + %d, rb);\n", A);
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(p->code[pc + 1]) + 2);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            // case OP_RETURN:
            case OP_RETURN1: {
                MCF("{\n");
                MCF("setobjs2s(L, L->top, base + %u);\n", A);
                MCF("api_incr_top(L);\n");
                MCF("luaC_checkGC(L);\n");
                MCF("return 1;\n");
                MCF("}\n");
                break;
            }
            case OP_RETURN0:
            {
                MCF("return 0;\n");
                break;
            }
            case OP_RETURN: {
                MCF("{\n");
                MCF("int n = %d - 1;\n", B);
                MCF("if (n < 0) \n");
                MCF("  n = cast_int(L->top - (base + %d));\n", A);
                MCF("for(int i = 0; i < n; ++i) {\n");
                MCF("  setobj2s(L, L->top, base + %d + i);\n", A);
                MCF("  api_incr_top(L);\n", A);
                MCF("}\n");
                MCF("return n;\n");
                MCF("}\n");
                break;
            }
            case OP_FORLOOP: {
                MCF("{\n");
                MCF("if (ttisinteger(s2v(base + %d + 2))) {  /* integer loop? */\n", A);
                MCF("  lua_Unsigned count = l_castS2U(ivalue(s2v(base + %d + 1)));\n", A);
                MCF("  if (count > 0) {  /* still more iterations? */\n");
                MCF("    lua_Integer step = ivalue(s2v(base + %d + 2));\n", A);
                MCF("    lua_Integer idx = ivalue(s2v(base + %d));  /* internal index */\n", A);
                MCF("    chgivalue(s2v(base + %d + 1), count - 1);  /* update counter */\n", A);
                MCF("    idx = intop(+, idx, step);  /* add step to index */\n");
                MCF("    chgivalue(s2v(base + %d), idx);  /* update internal index */\n", A);
                MCF("    setivalue(s2v(base + %d + 3), idx);  /* and control variable */\n", A);
                GEN_GOTO_OP(func_id, pc - GETARG_Bx(i) + 1);
                MCF("  }\n");
                MCF("}\n");
                MCF("else if (floatforloop(base + %d))  /* float loop */\n", A);
                GEN_GOTO_OP(func_id, pc - GETARG_Bx(i) + 1);
                MCF("}\n");
                break;
            }
            case OP_FORPREP: {
                MCF("{\n");
                MCF("if (forprep(L, base + %d))\n", A);
                GEN_GOTO_OP(func_id, pc + GETARG_Bx(i) + 2);
                MCF("}\n");
                break;
            }
            case OP_TFORPREP: {
                MCF("{\n");
                MCF("luaF_newtbcupval(L, base + %d + 3);\n", A);
                int jmp_step = B;
                GEN_GOTO_OP(func_id, pc + jmp_step + 2);
                MCF("}\n");
                break;
            }
            case OP_TFORCALL: {
                MCF("{\n");
                MCF("StkId ra = base + %d;\n", A);
                MCF("memcpy(ra + 4, ra, 3 * sizeof(*ra));\n");
                MCF("L->top = ra + 4 + 3;\n");
                MCF("luaD_callnoyield(L, ra + 4, %d);\n", C);
                MCF("}\n");
                break;
            }
            case OP_TFORLOOP: {
                MCF("{\n");
                MCF("StkId ra = base + %d;\n", A);
                MCF("if (!ttisnil(s2v(ra + 4))) {  /* continue loop? */\n");
                MCF("  setobjs2s(L, ra + 2, ra + 4);  /* save control variable */\n");
                GEN_GOTO_OP(func_id, pc - GETARG_Bx(i) + 1);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_SETLIST: {
                MCF("{\n");
                MCF("int n = %d;\n", B);
                MCF("unsigned int last = %d;\n", C);
                MCF("Table *h = hvalue(s2v(base + %d));\n", A);
                MCF("if (n == 0) \n");
                MCF("  n = cast_int(L->top - (base + %d)) - 1;\n", A);
                MCF("else\n");
                MCF("  L->top = ci->top;\n");
                MCF("last += n;\n");
                if (TESTARG_k(i)) {
                    MCF("last = %d * (MAXARG_C + 1);\n", GETARG_Ax(p->code[pc + 1]));
                }
                MCF("if (last > luaH_realasize(h))  /* needs more space? */\n");
                MCF("  luaH_resizearray(L, h, last);  /* preallocate it at once */");
                MCF("for (; n > 0; n--) {\n");
                MCF("  TValue *val = s2v(base + %d + n);\n", A);
                MCF("  setobj2t(L, &h->array[last - 1], val);\n");
                MCF("  last--;\n");
                MCF("  luaC_barrierback(L, h, val);\n");
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_CLOSURE: {
                MCF("{\n");
                Proto *next_p = p->p[GETARG_Bx(i)];
                Upvaldesc *uv = next_p->upvalues;
                lua_CFunction func = create_clua_func_from_lua(L, next_p, 0);
                if(func == NULL)
                {
                    luaL_error(L, "create_clua_func_from_lua failed");
                    return;
                }
                MCF("lua_CFunction next_func = (lua_CFunction)%lld;\n", (long long)func);
                MCF("int nup = %d;\n", next_p->sizeupvalues + 1);
                MCF("L->top = base + %d + 1;\n", A);
                for(int i = 0; i < next_p->sizeupvalues; i++)
                {
                    if(uv[i].instack)
                    {
                        MCF("setobj2s(L, L->top, s2v(base + %d));\n", uv[i].idx);
                        MCF("api_incr_top(L);\n");
                    }
                    else 
                    {
                        MCF("setobj2s(L, L->top, &func->upvalue[%d]);\n", uv[i].idx);
                        MCF("api_incr_top(L);\n");
                    }
                }
                MCF("lua_pushnil(L);\n");
                MCF("pushcclosure(L, next_func, nup, base + %d);\n", A);
                MCF("luaC_checkGC(L);\n");
                MCF("}\n");
                break;
            }
            // case OP_VARARG: {
            //     MCF("{\n");
            //     MCF("int n = %d - 1;  /* required results */\n", C);
            //     MCF("luaT_getvarargs(L, ci, base + %d, n);\n", A);
            //     MCF("}\n");
            //     break;
            // }
            // case OP_VARARGPREP: {
            //     MCF("{\n");
            //     MCF("luaT_adjustvarargs(L, %d, ci, or_func->p);\n", A);
            //     MCF("base = ci->func + 1;\n");
            //     MCF("}\n");
            //     break;
            // }
            case OP_VARARG:
            case OP_VARARGPREP:
                //printf("Unsupported opcode:  %-9s\n", opnames[GET_OPCODE(i)]);
                //assert(false);
                break;
            default:
                printf("Unknown opcode:  %-9s\n", opnames[GET_OPCODE(i)]);
                assert(false);
                break;
        }
    }
    if(scan_jump) {
        ctx->is_scan_jump = false;
    }
}

bool codegen_lua2c(lua_State* L, Proto *p, int func_id, LuaMirContext *ctx)
{
    opcode2c(L, ctx, func_id, p, true);
    MCF("#define LUA_LIB\n");
    for(size_t i = 0; i < LUA_HEADER_LIST_SIZE; i++) {
        MCF("#include \"%s\"\n", lua_header_list[i]);
    }
    MCF("#include \"lapi.h\"\n");
    MCF("#include \"math.h\"\n");
    MCF("#include \"luavm_utils.h\"\n");
    MCF("#define savestate(L,ci)		(L->top = ci->top)\n");
    MCF("#define Protect(exp)  (savestate(L,ci), (exp))\n");
    MCF("static int __jit_lfunc%d(lua_State *L) {\n", func_id);
    MCF("    CallInfo *ci = L->ci;\n");
    MCF("    StkId base = ci->func + 1;\n");
    MCF("    for(StkId skd = L->top; skd <= base; ++skd) {\n");
    MCF("       setnilvalue(s2v(skd));\n");
    MCF("    }\n");

//  CClosure *func = clCvalue(s2v(ci->func));
//       return (idx <= func->nupvalues) ? &func->upvalue[idx-1]
//                                       : &G(L)->nilvalue;


    MCF("     CClosure *func = clCvalue(s2v(ci->func));\n");
    // MCF("     LClosure *or_func = clLvalue(&func->upvalue[%d]);\n", cl->p->sizeupvalues);

//init k value
    MCF("Table *ktable = luaH_new(L);\n");
    MCF("{\n");
    MCF("luaH_resize(L, ktable, %d, 0);\n",p->sizek);
    MCF("TValue h;\n");
    MCF("sethvalue(L, &h, ktable);\n");
    MCF("TValue key;\n");
    MCF("TValue value;\n");
    MCF("const TValue* slot;\n");
    MCF("TString* ts;\n");
    for( int i = 0; i < p->sizek; ++i) {
        const TValue *kval = p->k + i;
        MCF("slot = NULL;\n");
        MCF("setivalue(&key, %d);\n", i + 1);
        switch (ttype(kval))
        {
            case LUA_TNUMBER: {
                if(ttisinteger(kval)) {
                    MCF("setivalue(&value, %lld);\n", ivalue(kval));
                }
                else {
                    MCF("setfltvalue(&value, %f);\n", fltvalue(kval));
                }
                break;
            }
            case LUA_TBOOLEAN: {
                if(!l_isfalse(kval)) {
                    MCF("setbtvalue(&value);\n");
                }
                else {
                    MCF("setbfvalue(&value);\n");
                }
                break;
            }
            case LUA_TNIL: {
                MCF("setnilvalue(&value);\n");
                break;
            }
            case LUA_TSTRING: {
                MCF("ts = luaS_new(L, ");
                const char *ts = svalue(kval);
                membuf_addquoted(&ctx->buf, ts, strlen(ts));
                MCF(");\n");
                MCF("setsvalue(L, &value, ts);\n");
                break;
            }
            default: {

            }
        }
        MCF("if (luaV_fastgeti(L, &h, %d, slot)) {\n", i + 1);
        MCF("   luaV_finishfastset(L, &h, slot, &value);\n");
        MCF("}\n");
        MCF("else {\n");
        MCF("  luaV_finishset(L, &h, &key, &value, slot);\n");
        MCF("}\n");
    }
    MCF("setobj2n(L, &func->upvalue[%d], &h);\n", p->sizeupvalues);
    MCF("luaC_barrier(L, func, &h);\n");
    
    MCF("}\n");


    MCF("     TValue *k = ktable->array;\n");
    #ifdef JITDEBUG
    MCF("     printf(\"func_id is %d\\n\");\n", func_id);
    #endif
    opcode2c(L, ctx, func_id, p, false);
    MCF("return 0;\n");
    MCF("}\n");
    return true;
}


int convert_lua_to_ccode(lua_State *L, Proto *p)
{
    if(!jit_cancompile(p)) {
        return 0;
    }
    char fname[64];
    int fun_id = get_jit_funcid(L);
    sprintf(fname, "__jit_lfunc%d", fun_id);
    LuaMirContext *ctx = luamir_ctx_new();
    if(!codegen_lua2c(L, p, fun_id, ctx)) {
        luamir_ctx_free(ctx);
        luaL_error(L, "codegen_lua2c failed");
        return 0;
    }
    const char *code = membuf_to_string(&ctx->buf);
    lua_pushstring(L, code);
    luamir_ctx_free(ctx);
    return 1;
}


lua_CFunction create_clua_func_from_lua(lua_State *L, Proto *p, int is_debug)
{
    if(!jit_cancompile(p)) {
        return NULL;
    }
    char fname[64];
    int fun_id = get_jit_funcid(L);
    sprintf(fname, "__jit_lfunc%d", fun_id);
    LuaMirContext *ctx = luamir_ctx_new();
    if(!codegen_lua2c(L, p, fun_id, ctx)) {
        luamir_ctx_free(ctx);
        luaL_error(L, "codegen_lua2c failed");
        return NULL;
    }
    const char *code = membuf_to_string(&ctx->buf);
    #ifdef JITDEBUG
    printf("%s\n", code);
    #endif
    lua_CFunction func = create_clua_func_from_c(L, fname, code);
    luamir_ctx_free(ctx);
    if(is_debug) {
        func =  create_test_func(L);
    }
    return func;

}