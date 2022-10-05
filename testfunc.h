#ifndef TESTFUNC_H
#define TESTFUNC_H
#include <lua.h>
#include <lobject.h>

lua_CFunction create_test_func(lua_State *L);
#endif