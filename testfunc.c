#include "testfunc.h"
#include <lauxlib.h>
#include <lualib.h>
#include <lobject.h>
#include <lstate.h>
#include <lgc.h>
#include <luaconf.h>
#include <lapi.h>
#include <llimits.h>
#include <ltable.h>
#include <lvm.h>
static int __jit_lfunc13(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    CClosure *func = clCvalue(s2v(ci->func));
    LClosure *or_func = clLvalue(func->upvalue);
    TValue *k = or_func->p->k;

{
TValue *upval = or_func->upvals[0]->v;
TValue *rc = k;
TString *key = tsvalue(rc);
const TValue *aslotx = luaH_getshortstr(hvalue(upval), key);
const TValue *slotx = &(hvalue(upval)->node->i_val);
setobj2s(L,  base+0, slotx);
}

{
setobjs2s(L, L->top, base + 0);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}

return 0;
}

lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc13;
}