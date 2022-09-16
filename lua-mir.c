#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "c2luafun.h"


static int l_cjit(lua_State *L)
{
    const char *souce = luaL_checkstring(L, 1);
    const char *code = luaL_checkstring(L, 2);
    printf("code: %s\n", code);
    lua_CFunction func = create_lua_func_from_c(L, "test", code);
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

