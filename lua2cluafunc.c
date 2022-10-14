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


#define GEN_RKC(i) int testk = TESTARG_k(i);\
                if(testk) { \
                    MCF("TValue *rc = k + %d;\n", C); \
                } else { \
                    MCF("TValue *rc = s2v(base+%d);\n", C); \
                }

bool codegen_lua2c(lua_State *L, LClosure *cl, int func_id, Membuf *buf)
{
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
    MCF("     LClosure *or_func = clLvalue(func->upvalue);\n");
    MCF("     TValue *k = or_func->p->k;\n");
    TValue *k = cl->p->k;
    for(int pc = 0; pc < cl->p->sizecode; ++pc)
    {
        const Instruction i = cl->p->code[pc];
        OpCode op = GET_OPCODE(i);
        int A = GETARG_A(i);
        int B = GETARG_B(i);
        int C = GETARG_C(i);
        MCF("__jitfunc%d_op%d: \n", func_id, pc);
        MCF("//opcode is : %-9s\n", opnames[op]);
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
                parse_op_loadk(buf, A, GETARG_Bx(i), k);
                break;
            }
            case OP_LOADKX: {
                parse_op_loadk(buf, A, GETARG_Ax(cl->p->code[++pc]), k);
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
                MCF("setobj2s(L, base+%d, or_func->upvals[b]->v);\n", A);
                MCF("}\n");
                break;
            }
            case OP_SETUPVAL: {
                MCF("{\n");
                MCF("int b = %d;\n", B);
                MCF("UpVal *uv = or_func->upvals[b];\n");
                MCF("setobj(L, uv->v, s2v(base+%d));\n", A);
                MCF("luaC_barrier(L, uv, s2v(base+%d));", A);
                MCF("}\n");
                break;
            }
            case OP_GETTABUP: {
                MCF("{\n");
                MCF("const TValue *slot;\n");
                MCF("int b = %d;\n", B);
                MCF("int c = %d;\n", C);
                MCF("TValue *upval = or_func->upvals[b]->v;\n");
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
                MCF("TValue *upval = or_func->->upvals[%d]->v;", A);
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
                MCF("   luaV_finishset(L, s2v(base + %d), rb, rc, slot);\n");
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
                MCF("  b = 1 << (b - 1);  /* size is 2^(b - 1) */");
                if(TESTARG_k(i))
                    MCF("c += %d * (%d + 1);\n", GETARG_Ax(cl->p->code[i+1]), MAXARG_C);
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
                parse_op_arithI(func_id, pc, buf, '+', A, B, GETARG_sC(i));
                break;
            }
            case OP_ADDK: {
                parse_op_arithK(func_id, pc, buf, '+', A, B, C);
                break;
            }
            case OP_SUBK: {
                parse_op_arithK(func_id, pc, buf, '-', A, B, C);
                break;
            }
            case OP_MULK: {
                parse_op_arithK(func_id, pc, buf, '*', A, B, C);
                break;
            }
            case OP_MODK: {
                parse_op_arithK(func_id, pc, buf, '%', A, B, C);
                break;
            }
            case OP_POWK: {
                parse_op_arithKf(func_id, pc, buf, '^', A, B, C);
                break;
            }
            case OP_DIVK: {
                parse_op_arithKf(func_id, pc, buf, '/', A, B, C);
                break;
            }
            case OP_IDIVK: {
                parse_op_arithK(func_id, pc, buf, '\\', A, B, C);
                break;
            }
            case OP_BANDK: {
                parse_op_bitwiseK(func_id, pc, buf, '&', A, B, C);
                break;
            }
            case OP_BORK: {
                parse_op_bitwiseK(func_id, pc, buf, '|', A, B, C);
                break;
            }
            case OP_BXORK: {
                parse_op_bitwiseK(func_id, pc, buf, '^', A, B, C);
                break;
            }
            case OP_SHRI: {
                parse_op_shiftI(func_id, pc, buf, '>', A, B, GETARG_sC(i));
                break;
            }
            case OP_SHLI: {
                parse_op_shiftI(func_id, pc, buf, '<', A, B, GETARG_sC(i));
                break;
            }
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
            case OP_BAND: {
                parse_op_bitwise(func_id, pc, buf, '&', A, B, C);
                break;
            }
            case OP_BOR: {
                parse_op_bitwise(func_id, pc, buf, '|', A, B, C);
                break;
            }
            case OP_BXOR: {
                parse_op_bitwise(func_id, pc, buf, '^', A, B, C);
                break;
            }
            case OP_SHR: {
                parse_op_bitwise(func_id, pc, buf, '>', A, B, C);
                break;
            }
            case OP_SHL: {
                parse_op_bitwise(func_id, pc, buf, '<', A, B, C);
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
            case OP_MMBINI: {
                Instruction pi = cl->p->code[pc - 1];
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
                Instruction pi = cl->p->code[pc - 1];
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
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(cl->p->code[pc + 1]) + 2);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_LT: {
                parse_op_order(func_id, pc, buf, A, B, GETARG_k(i), GETARG_sJ(cl->p->code[pc + 1]), "l_lti", "LTnum", "lessthanothers");
                break;
            }
            case OP_LE: {
                parse_op_order(func_id, pc, buf, A, B, GETARG_k(i), GETARG_sJ(cl->p->code[pc + 1]), "l_lei", "LEnum", "lessequalothers");
                break;
            }
            case OP_EQK : {
                MCF("{\n");
                MCF("TValue *rb = k + %d;\n", B);
                MCF("int cond = luaV_rawequalobj(s2v(base + %d), rb);\n", A);
                MCF("if (cond != %d) {\n", GETARG_k(i));
                GEN_GOTO_OP(func_id, pc + 2);
                MCF("} else {\n");
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(cl->p->code[pc + 1]) + 2);
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
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(cl->p->code[pc + 1]) + 2);
                MCF("}\n");
                MCF("}\n");
                break;
            }
            case OP_LTI: {
                parse_op_orderI(func_id, pc, buf, A, B, C, GETARG_k(i), GETARG_sJ(cl->p->code[pc + 1]), "l_lti", "luai_numlt", "0", "TM_LT");
                break;
            }
            case OP_LEI: {
                parse_op_orderI(func_id, pc, buf, A, B, C, GETARG_k(i), GETARG_sJ(cl->p->code[pc + 1]), "l_lei", "luai_numle", "0", "TM_LE");
                break;
            }
            case OP_GTI: {
                parse_op_orderI(func_id, pc, buf, A, B, C, GETARG_k(i), GETARG_sJ(cl->p->code[pc + 1]), "l_gti", "luai_numgt", "1", "TM_LT");
                break;
            }
            case OP_GEI: {
                parse_op_orderI(func_id, pc, buf, A, B, C, GETARG_k(i), GETARG_sJ(cl->p->code[pc + 1]), "l_gei", "luai_numge", "1", "TM_LE");
                break;
            }
            case OP_TEST: {
                MCF("{\n");
                MCF("int cond = !l_isfalse(s2v(base + %d));\n", A);
                MCF("if (cond != %d) {\n", GETARG_k(i));
                GEN_GOTO_OP(func_id, pc + 2);
                MCF("} else {\n");
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(cl->p->code[pc + 1]) + 2);
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
                GEN_GOTO_OP(func_id, pc + GETARG_sJ(cl->p->code[pc + 1]) + 2);
                MCF("}\n");
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
            case OP_RETURN0:
            {
                break;
            }
            default:
                printf("Unknown opcode:  %-9s\n", opnames[GET_OPCODE(i)]);
                assert(false);
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
    // return create_test_func(L);

}