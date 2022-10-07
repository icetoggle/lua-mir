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
static int __jit_lfunc25(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    for(StkId skd = L->top; skd <= base; ++skd) {
       setnilvalue(s2v(skd));
    }
     CClosure *func = clCvalue(s2v(ci->func));
     LClosure *or_func = clLvalue(func->upvalue);
     TValue *k = or_func->p->k;
__jitfunc25_op0: 
//opcode is : ADDK     
{
TValue *v1 = s2v(base + 0);
TValue *v2 = k + 0;
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 1), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc25_op2;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
        setfltvalue(s2v(base + 1), n1 + n2);
    goto __jitfunc25_op2;
    }
}
}
__jitfunc25_op1: 
//opcode is : MMBINK   
{
TValue *imm = k + 0;
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 1;
luaT_trybinassocTM(L, s2v(base + 0), imm, flip, result, tm);
}
__jitfunc25_op2: 
//opcode is : RETURN1  
{
setobjs2s(L, L->top, base + 1);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
__jitfunc25_op3: 
//opcode is : RETURN0  
return 0;
}

lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc25;
}