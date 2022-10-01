#include "mir_utils.h"
#include <lua.h>
#include <lauxlib.h>
#include <mir.h>
#include <c2mir/c2mir.h>
#include <mir-varr.h>
#include <string.h>



static int mir_context_gc(lua_State *L)
{
    mir_ctx_wrapper *ctx_wrapper = (mir_ctx_wrapper *)lua_touserdata(L, 1);
    MIR_finish(ctx_wrapper->ctx);
    return 0;
}

typedef const char *char_ptr_t;

DEF_VARR (char_ptr_t);

mir_ctx_wrapper* init_mir_context(lua_State *L)
{
    lua_pushstring(L, "__mir_context");
    lua_gettable(L, LUA_REGISTRYINDEX);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        MIR_context_t ctx = MIR_init();
        lua_pushstring(L, "__mir_context");
        mir_ctx_wrapper *wrapper = lua_newuserdata(L, sizeof(mir_ctx_wrapper));
        wrapper->ctx = ctx;
        memset(&wrapper->options, 0, sizeof(wrapper->options));
        VARR(char_ptr_t) *headers;
        VARR_CREATE(char_ptr_t, headers, 0);
        VARR_PUSH(char_ptr_t, headers, "./lua-src");
        wrapper->options.include_dirs = VARR_ADDR(char_ptr_t, headers);
        wrapper->options.include_dirs_num = 1;
        luaL_newmetatable(L, "mir_context");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, mir_context_gc);
        lua_settable(L, -3);
        lua_setmetatable(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);
        return wrapper;
    } else {
        mir_ctx_wrapper* ctx = lua_touserdata(L, -1);
        lua_pop(L, 1);
        return ctx;
    }
    return NULL;
}