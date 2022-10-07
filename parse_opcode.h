#ifndef PARSE_OPCODE_H
#define PARSE_OPCODE_H
#include "membuf.h"
#include <lobject.h>

#define GEN_GOTO_OP(func_id, pc_idx)    MCF("    goto __jitfunc%d_op%d;\n", (func_id), (pc_idx));
void parse_op_arith(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C);
void parse_op_arithf(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C);

void parse_op_arithI(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int imm);
void parse_op_arithK(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C);
void parse_op_arithKf(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C);
void parse_op_bitwiseK(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C);
void parse_op_shiftI(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int ic);
void parse_op_loadk(Membuf *buf, int A, int bx, TValue *k);

#endif 