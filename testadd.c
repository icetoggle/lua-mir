#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <lobject.h>
#include <lstate.h>
#include <lgc.h>
#include <luaconf.h>
#include <lapi.h>
#include <llimits.h>
#include <lvm.h>
static int __jit_lfunc0(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
{
TValue *v1 = s2v(base + 0);
TValue *v2 = s2v(base + 1);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 2), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 2), n1 + n2);
    }
}
}
{
setobjs2s(L, L->top, base + 2);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
}



int luaopen_testadd(lua_State *L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"add", __jit_lfunc0},
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}