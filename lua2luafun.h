#ifndef LUA2LUAFUN_H
#define LUA2LUAFUN_H
#include "lua.h"

lua_CFunction create_lua_func_from_lua(lua_State *L, const char *source_name, const char* code);

#endif