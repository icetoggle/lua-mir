#ifndef C2LUAFUN_H
#define C2LUAFUN_H

#include <lualib.h>

lua_CFunction create_lua_func_from_c(lua_State *L, const char *source_name, const char* code);


#endif