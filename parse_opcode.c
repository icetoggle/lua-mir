#include "parse_opcode.h"

void parse_op_arith(int func_id, int pc_idx, Membuf *buf, char op, int A, int B, int C)
{
    MCF("{\n");
    MCF("TValue *v1 = s2v(base + %u);\n", B);
    MCF("TValue *v2 = s2v(base + %u);\n", C);
    MCF("if(ttisinteger(v1) && ttisinteger(v2)) {\n");
    MCF("    lua_Integer i1 = ivalue(v1);\n");
    MCF("    lua_Integer i2 = ivalue(v2);\n");

    MCF("    setivalue(s2v(base + %u), l_castU2S(l_castS2U(i1) %c l_castS2U(i2)));\n", A, op);
    MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    MCF("} else {\n");
    MCF("    lua_Number n1,n2;\n");
    MCF("    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {\n");
    MCF("       setfltvalue(s2v(base + %d), n1 %c n2);\n", A, op);
    MCF("    goto __jitfunc%d_op%d;\n", func_id, pc_idx + 2);
    MCF("    }\n");
    MCF("}\n");
    MCF("}\n");
}