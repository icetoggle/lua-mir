#include "testfunc.h"
#include "lauxlib.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lua.h"
#include "lundump.h"
#include "lvm.h"
#include "ltm.h"
#include "lapi.h"
#define savestate(L,ci)         (L->top = ci->top)
#define Protect(exp)  (savestate(L,ci), (exp))
static int __jit_lfunc15(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
     CClosure *func = clCvalue(s2v(ci->func));
     LClosure *or_func = clLvalue(func->upvalue);
     TValue *k = or_func->p->k;
__jitfunc15_op0: 
{
const TValue *slot;
TValue *rb = s2v(base+0);
TValue *rc = k+0;
TString *key =tsvalue(rc);
if (luaV_fastget(L, rb, key, slot, luaH_getshortstr)) {
   setobj2s(L, base+1, slot);
} else {
   luaV_finishget(L, rb, rc,  base+1, slot);
}
}
__jitfunc15_op1: 
{
setobjs2s(L, L->top, base + 1);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
__jitfunc15_op2: 
return 0;
}

lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc15;
}