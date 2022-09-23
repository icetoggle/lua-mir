#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <lobject.h>
#include <lstate.h>
#include <lgc.h>
#include <luaconf.h>

static int __jit_lfunc0(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    Instruction *pc;
{
    TValue *v1 = s2v(base + 2);
    TValue *v2 = s2v(base + 1);
    if(ttisinteger(v1) && ttisinteger(v2)) {
        lua_Integer i1 = ivalue(v1);
        lua_Integer i2 = ivalue(v2);
        setivalue(s2v(base + 2), i1 + i2);
    } else if(ttisfloat(v1)) {
        lua_Number n1 = nvalue(v1);
        lua_Number n2 = nvalue(v2);
        setfltvalue(s2v(base + 2), n1 + n2);
    }
}
{
// int nres = ci->nresults;
// L->ci = ci->previous;
// if (nres == 0)
//    L->top = base - 1;
// else {
//    setobjs2s(L, base - 1, base + 2);
//    L->top = base;
//    for (; luai_unlikely(nres > 1); nres--) {
//        setnilvalue(s2v(L->top++));
//    }
// }
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