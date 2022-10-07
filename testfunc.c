#define LUA_LIB
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
#include "math.h"
#define savestate(L,ci)         (L->top = ci->top)
#define Protect(exp)  (savestate(L,ci), (exp))
static int __jit_lfunc46(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    for(StkId skd = L->top; skd <= base; ++skd) {
       setnilvalue(s2v(skd));
    }
     CClosure *func = clCvalue(s2v(ci->func));
     LClosure *or_func = clLvalue(func->upvalue);
     TValue *k = or_func->p->k;
__jitfunc46_op0: 
//opcode is : EQ       
{
int cond;
TValue *rb = s2v(base + 1);
cond = luaV_equalobj(L, s2v(base + 0), rb);
if (cond != 1) {
    goto __jitfunc46_op2;
} else {
    goto __jitfunc46_op2;
}
}
{
setobjs2s(L, L->top, base + 0);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
__jitfunc46_op1: 
//opcode is : JMP      
    goto __jitfunc46_op2;
__jitfunc46_op2: 
//opcode is : LFALSESKIP
setbfvalue(s2v(base+2));
    goto __jitfunc46_op4;
__jitfunc46_op3: 
//opcode is : LOADTRUE 
setbtvalue(s2v(base+2));
__jitfunc46_op4: 
//opcode is : RETURN1  
{
setobjs2s(L, L->top, base + 2);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
__jitfunc46_op5: 
//opcode is : RETURN0  
return 0;
}
lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc46;
}