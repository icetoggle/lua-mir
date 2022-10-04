#ifndef PARSE_OPCODE_H
#define PARSE_OPCODE_H
#include "membuf.h"
#include <lobject.h>
void parse_op_arith(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C);
void parse_op_arithf(int func_id, int pc_idx, Membuf *mb, char op, int A, int B, int C);
void parse_op_loadk(Membuf *buf, int A, int bx, TValue *k);
#endif