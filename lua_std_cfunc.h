#ifndef LUA_STD_CFUNC_H
#define LUA_STD_CFUNC_H

void *import_luacfun_resolver(const char *name);
#define LUA_HEADER_LIST_SIZE 15
const char* lua_header_list[LUA_HEADER_LIST_SIZE];
#endif