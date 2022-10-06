#include "parse_opcode.h"


const char *arithi_opcode(char op) 
{
    static char buf[64];
    switch(op) {
        case '+':
        case '-':
        case '*': {
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



void parse_op_arith(int func_id, int pc_idx, Membuf *buf, char op, int A, int B, int C)
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

void parse_op_arithf(int func_id, int pc_idx, Membuf *buf, char op, int A, int B, int C)
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

void parse_op_arithI(int func_id, int pc_idx, Membuf *buf, char op, int A, int B, int imm)
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

void parse_op_arithK(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C)
{

}

void parse_op_loadk(Membuf *buf, int A, int bx, TValue *k)
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