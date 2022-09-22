#ifndef C2CLUAFUNC_H
#define C2CLUAFUNC_H

#include <lualib.h>

lua_CFunction create_clua_func_from_c(lua_State *L, const char *source_name, const char* code);


#endif