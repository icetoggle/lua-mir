#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <lundump.h>
#include "c2cluafunc.h"
#include "lua2cluafunc.h"
#include <string.h>
#include <stdlib.h>

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



static int l_getlclosure(lua_State *L)
{
    LClosure *cl = lua_tolclosure(L, 1);
    if(cl == NULL) {
        return 0;
    }
    lua_pushlclosure(L, cl);
    return 1;
}

static int l_ljit(lua_State *L)
{
    LClosure *cl = lua_tolclosure(L, 1);
    if(cl == NULL) {
        return 0;
    }
    lua_CFunction func = create_clua_func_from_lua(L, cl);
    if(func == NULL) {
        return 0;
    }
    lua_pushcfunction(L, func);
    return 1;
}

static int l_lua2c(lua_State *L)
{
    LClosure *cl = lua_tolclosure(L, 1);
    if(cl == NULL) {
        return 0;
    }
    convert_lua_to_ccode(L, cl);
    return 1;
}

int luaopen_luamir(lua_State *L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"cjit", l_cjit},
        {"getlclosure", l_getlclosure},
        {"lua2c", l_lua2c},
        {"ljit", l_ljit},
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}

