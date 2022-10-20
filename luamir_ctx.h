#ifndef LUAMIR_CTX_H
#define LUAMIR_CTX_H
#include "membuf.h"
#include "array.h"
typedef struct {
    Membuf buf;
    Array exist_jumps;
    bool is_scan_jump;
} LuaMirContext;

LuaMirContext *luamir_ctx_new();
void luamir_ctx_free(LuaMirContext *ctx);

#endif