#ifndef PARSE_OPCODE_H
#define PARSE_OPCODE_H
#include "membuf.h"
#include <lobject.h>
#include "luamir_ctx.h"

#define GEN_GOTO_OP(func_id, pc_idx) if(ctx->is_scan_jump) array_append(&ctx->exist_jumps, (pc_idx) + 1); \
                                    MCF("    goto __jitfunc%d_op%d;\n", (func_id), (pc_idx) + 1);
void parse_op_arith(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int C);
void parse_op_arithf(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int C);
void parse_op_bitwise(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int C);
void parse_op_order(int func_id, int pc_idx, LuaMirContext *mb, int A, int B, int k, int next_sj, const char* opi, const char *opn, const char *other);
void parse_op_orderI(int func_id, int pc_idx, LuaMirContext *buf, int A, int B, int C, int k, int next_sj, const char* opi, const char* opf, const char* inv, const char* tm);

void parse_op_arithI(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int imm);
void parse_op_arithK(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int C);
void parse_op_arithKf(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int C);
void parse_op_bitwiseK(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int C);
void parse_op_shiftI(int func_id, int pc_idx, LuaMirContext *mb, char op, int A, int B, int ic);
void parse_op_loadk(LuaMirContext *buf, int A, int bx, TValue *k);

#endif 