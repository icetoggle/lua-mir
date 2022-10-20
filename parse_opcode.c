#include "parse_opcode.h"
#include "lopcodes.h"


const char *arithi_opcode(char op) 
{
    static char buf[64];
    switch(op) {
        case '+':
        case '-':
        case '*': 
        case '&':
        case '|':
        case '^':{
            sprintf(buf, "l_castU2S(l_castS2U(i1) %c l_castS2U(i2))", op);
            return buf;
        }
        case '%': {
            sprintf(buf, "luaV_mod(L, i1, i2)");
            return buf;
        }
        case '\\': {
            sprintf(buf, "luaV_idiv(L, i1, i2)");
            return buf;
        }
        case '>': {
            sprintf(buf, "luaV_shiftl(i1, -i2)");
            return buf;
        }
        case '<': {
            sprintf(buf, "luaV_shiftl(i1, i2)");
            return buf;
        }
        default: return NULL;
    }
}

const char *arithf_opcode(char op)
{
    static char buf[64];
    switch (op)
    {
    case '+':
    case '-':
    case '*': {
        sprintf(buf, "n1 %c n2", op);
        return buf;
    }
    case '%': {
            sprintf(buf, "luaV_modf(L, n1, n2)");
            return buf;
    }
    case '^': {
        sprintf(buf, "luai_numpow(L, n1, n2)");
        return buf;
    }
    case '/': {
        sprintf(buf, "luai_numdiv(L, n1, n2)");
        return buf;
    }
    case '\\': {
        sprintf(buf, "luai_numidiv(L, n1, n2)");
        return buf;
    }
    default:
        break;
    }
    return NULL;
}



void parse_op_arith(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int C)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %u);\n", B);
    MCF("TValue *v2 = s2v(base + %u);\n", C);
    MCF("if(ttisinteger(v1) && ttisinteger(v2)) {\n");
    MCF("    lua_Integer i1 = ivalue(v1);\n");
    MCF("    lua_Integer i2 = ivalue(v2);\n");

    MCF("    setivalue(s2v(base + %u), %s);\n", A, arithi_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("} else {\n");
    MCF("    lua_Number n1,n2;\n");
    MCF("    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {\n");
    MCF("       setfltvalue(s2v(base + %d), %s);\n", A, arithf_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("    }\n");
    MCF("}\n");
    MCF("}\n");
}

void parse_op_arithf(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int C)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %u);\n", B);
    MCF("TValue *v2 = s2v(base + %u);\n", C);
    MCF("lua_Number n1,n2;\n");
    MCF("if (tonumberns(v1, n1) && tonumberns(v2, n2)) {\n");
    MCF("    setfltvalue(s2v(base + %d), %s);\n", A, arithf_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("}\n");
    MCF("}\n");
}

void parse_op_bitwise(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int C)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %u);\n", B);
    MCF("TValue *v2 = s2v(base + %u);\n", C);
    MCF(" lua_Integer i1, i2;\n");
    MCF("if(tointegerns(v1, &i1) && tointegerns(v2, &i2)) {\n");
    MCF("    setivalue(s2v(base + %u), %s);\n", A, arithi_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("}\n");
    MCF("}\n");
}


void parse_op_order(int func_id, int pc_idx, LuaMirContext *ctx, int A, int B, int k, int next_sj, const char* opi, const char *opn, const char *other)
{
    MCF("{\n");
    MCF("int cond;\n");
    MCF("TValue *rb = s2v(base+%d);\n", B);
    MCF("if (ttisinteger(s2v(base+%d)) && ttisinteger(rb)) {\n", A);
    MCF("  lua_Integer ia = ivalue(s2v(base+%d));\n", A);
    MCF("  lua_Integer ib = ivalue(rb);\n");
    MCF("  cond = %s(ia,ib);\n", opi);
    MCF("} else if (ttisnumber(s2v(base+%d)) && ttisnumber(rb)) {\n", A);
    MCF("  cond = %s(s2v(base+%d), rb); \n", opn, A);
    MCF("} else {\n");
    MCF("  cond = %s(L, s2v(base+%d), rb);\n", other, A);
    MCF("}\n");
    MCF("if (cond != %d)\n", k);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("else \n");
    GEN_GOTO_OP(func_id, pc_idx + next_sj + 2);
    MCF("}\n");
}

void parse_op_orderI(int func_id, int pc_idx, LuaMirContext *ctx, int A, int B, int C, int k, int next_sj, const char* opi, const char* opf, const char* inv, const char* tm)
{
    MCF("{\n");
    MCF("int cond;\n");
    MCF("int im = %d;\n", sC2int(B));
    MCF("if (ttisinteger(s2v(base + %d))) \n", A);
    MCF("  cond = %s(ivalue(s2v(base + %d)), im);\n", opi, A);
    MCF("else if (ttisfloat(s2v(base + %d))) {\n", A);
    MCF("  lua_Number fa = fltvalue(s2v(base + %d));\n", A);
    MCF("  lua_Number fim = cast_num(im);\n", opi);
    MCF("  cond = %s(fa, fim);\n", opf);
    MCF("} else {\n");
    MCF("  int isf = %d;\n", C);
    MCF("  cond = luaT_callorderiTM(L, s2v(base + %d), im, %s, isf, %s);\n", A, inv, tm);
    MCF("}\n");
    MCF("if (cond != %d)\n", k);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("else \n");
    GEN_GOTO_OP(func_id, pc_idx + next_sj + 2);
    MCF("}\n");
}
void parse_op_arithI(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int imm)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %d);\n", B);
    MCF("if(ttisinteger(v1)) {\n");
    MCF("    lua_Integer i1 = ivalue(v1);\n");
    MCF("    lua_Integer i2 = %d;\n", imm);
    MCF("    setivalue(s2v(base + %d), %s);\n", A, arithi_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("}\n");
    MCF("else if (ttisfloat(v1)) {\n");
    MCF("    lua_Number n1 = fltvalue(v1);\n");
    MCF("    lua_Number n2 = cast_num(%d);\n", imm);
    MCF("    setfltvalue(s2v(base + %d), %s);\n", A, arithf_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("}\n");
    MCF("}\n");
}

void parse_op_arithK(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int C)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %d);\n", B);
    MCF("TValue *v2 = k + %d;\n", C);
    MCF("if(ttisinteger(v1) && ttisinteger(v2)) {\n");
    MCF("    lua_Integer i1 = ivalue(v1);\n");
    MCF("    lua_Integer i2 = ivalue(v2);\n");
    MCF("    setivalue(s2v(base + %d), %s);\n", A, arithi_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("} else {\n");
    MCF("    lua_Number n1,n2;\n");
    MCF("    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {\n");
    MCF("        setfltvalue(s2v(base + %d), %s);\n", A, arithf_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("    }\n");
    MCF("}\n");
    MCF("}\n");
}


void parse_op_arithKf(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int C)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %d);\n", B);
    MCF("TValue *v2 = k + %d;\n", C);
    MCF("lua_Number n1,n2;\n");
    MCF("if (tonumberns(v1, n1) && tonumberns(v2, n2)) {\n");
    MCF("    setfltvalue(s2v(base + %d), %s);\n", A, arithf_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("}\n");
    MCF("}\n");
}

void parse_op_bitwiseK(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int C)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %d);\n", B);
    MCF("TValue *v2 = k + %d;\n", C);
    MCF("lua_Integer i1;\n");
    MCF("lua_Integer i2 = ivalue(v2);\n");
    MCF("if(tointegerns(v1, &i1)) {\n");
    MCF("    setivalue(s2v(base + %d), %s);\n", A, arithi_opcode(op));
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("}\n");
    MCF("}\n");
}

void parse_op_shiftI(int func_id, int pc_idx, LuaMirContext *ctx, char op, int A, int B, int ic)
{
    MCF("{\n");
    MCF("TValue *rb = s2v(base + %d);\n", B);
    MCF("lua_Integer ib;\n");
    MCF("if(tointegerns(rb, &ib)) {\n");
    if(op == '>')
    {
        MCF("    setivalue(s2v(base + %d), luaV_shiftl(ib, -(%d)));\n", A, ic);
    }
    else if(op == '<')
    {
        MCF("    setivalue(s2v(base + %d), luaV_shiftl((%d), ib));\n", A, ic);
    }
    // MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    GEN_GOTO_OP(func_id, pc_idx + 2);
    MCF("}\n");
    MCF("}\n");
}

void parse_op_loadk(LuaMirContext *ctx, int A, int bx, TValue *k)
{
    TValue* b = k + bx;
    MCF("{\n");
    switch (b->tt_)
    {
    case LUA_VNUMINT: {
        MCF("setivalue(s2v(base + %d), %lld);\n", A, b->value_.i);
        break;
    }
    case LUA_VNUMFLT: {
        MCF("setfltvalue(s2v(base + %d), %f);\n", A, b->value_.n);
        break;
    }
    case LUA_VFALSE: {
        MCF("setbtvalue(s2v(base + %d));\n", A);
        break;
    }
    case LUA_VTRUE: {
        MCF("setbfvalue(s2v(base + %d));\n", A);
        break;
    }
    default:
    {
        MCF("setobj2s(L, base + %d, k + %d);\n", A, bx);
        break;
    }
    }
    MCF("}\n");
}