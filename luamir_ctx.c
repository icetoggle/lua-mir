#include "luamir_ctx.h"
#include <stdlib.h>

LuaMirContext* luamir_ctx_new()
{
    LuaMirContext *ctx = malloc(sizeof(LuaMirContext));
    membuf_init(&ctx->buf);
    array_init(&ctx->exist_jumps, 16);
    ctx->is_scan_jump = false;
    return ctx;
}

void luamir_ctx_free(LuaMirContext *ctx)
{
    membuf_free(&ctx->buf);
    array_free(&ctx->exist_jumps);
    free(ctx);
}