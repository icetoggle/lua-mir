#ifndef LUA_2_CLUA_FUNC_H
#define LUA_2_CLUA_FUNC_H
#include <lua.h>
#include <lobject.h>

LClosure *lua_tolclosure(lua_State *L, int idx);

LClosure *lua_pushlclosure(lua_State *L, LClosure *cl);

int convert_lua_to_ccode(lua_State *L, LClosure *cl);

lua_CFunction create_clua_func_from_lua(lua_State *L, LClosure *cl);


#endif