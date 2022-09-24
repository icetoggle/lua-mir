#ifndef PARSE_OPCODE_H
#define PARSE_OPCODE_H
#include "membuf.h"
void parse_op_arith(Membuf *mb, char op, int A, int B, int C);
#endif