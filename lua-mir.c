#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "c2cluafunc.h"


static int l_cjit(lua_State *L)
{
    const char *source = luaL_checkstring(L, 1);
    const char *code = luaL_checkstring(L, 2);
    lua_CFunction func = create_clua_func_from_c(L, source, code);
    if(func == NULL) {
        return 0;
    }
    lua_pushcfunction(L, func);
    return 1;
}

int luaopen_luamir(lua_State *L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"cjit", l_cjit},
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}

