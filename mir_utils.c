#include "mir_utils.h"
#include <lua.h>
#include <lauxlib.h>
#include <mir.h>

typedef struct {
    MIR_context_t ctx;
}mir_ctx_wrapper;

static int mir_context_gc(lua_State *L)
{
    mir_ctx_wrapper *ctx_wrapper = (mir_ctx_wrapper *)lua_touserdata(L, 1);
    MIR_finish(ctx_wrapper->ctx);
    return 0;
}

MIR_context_t init_mir_context(lua_State *L)
{
    lua_pushstring(L, "__mir_context");
    lua_gettable(L, LUA_REGISTRYINDEX);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        MIR_context_t ctx = MIR_init();
        lua_pushstring(L, "__mir_context");
        mir_ctx_wrapper *wrapper = lua_newuserdata(L, sizeof(mir_ctx_wrapper));
        wrapper->ctx = ctx;
        luaL_newmetatable(L, "mir_context");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, mir_context_gc);
        lua_settable(L, -3);
        lua_setmetatable(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);
        return ctx;
    } else {
        MIR_context_t ctx = lua_touserdata(L, -1);
        lua_pop(L, 1);
        return ctx;
    }
    return NULL;
}