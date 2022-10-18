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
#include "luavm_utils.h"
#define savestate(L,ci)         (L->top = ci->top)
#define Protect(exp)  (savestate(L,ci), (exp))
static int __jit_lfunc1(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    for(StkId skd = L->top; skd <= base; ++skd) {
       setnilvalue(s2v(skd));
    }
     CClosure *func = clCvalue(s2v(ci->func));
     LClosure *or_func = clLvalue(func->upvalue);
     TValue *k = or_func->p->k;
__jitfunc1_op0: 
puts("//opcode is : GETTABUP \n");
//opcode is : GETTABUP 
{
const TValue *slot;
int b = 0;
int c = 0;
TValue *upval = or_func->upvals[b]->v;
TValue *rc = k+c;
TString *key = tsvalue(rc);
if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {
   setobj2s(L,  base+2, slot);
} else {
   luaV_finishget(L, upval, rc,  base+2, slot);
}
}
__jitfunc1_op1: 
puts("//opcode is : MOVE     \n");
//opcode is : MOVE     
{
setobjs2s(L, base + 3, base + 0);
}
__jitfunc1_op2: 
puts("//opcode is : CALL     \n");
//opcode is : CALL     
{
int b = 2;
int nresults = 1 - 1;
if (b != 0) L->top = (base + 2) + b;
luaD_callnoyield(L, base + 2, nresults);
adjustresults(L, nresults);
}
__jitfunc1_op3: 
puts("//opcode is : GETUPVAL \n");
//opcode is : GETUPVAL 
{
int b = 1;
setobj2s(L, base+2, or_func->upvals[b]->v);
}
__jitfunc1_op4: 
puts("//opcode is : MOVE     \n");
//opcode is : MOVE     
{
setobjs2s(L, base + 3, base + 0);
}
__jitfunc1_op5: 
puts("//opcode is : MOVE     \n");
//opcode is : MOVE     
{
setobjs2s(L, base + 4, base + 1);
}
__jitfunc1_op6: 
puts("//opcode is : CALL     \n");
//opcode is : CALL     
{
int b = 3;
int nresults = 2 - 1;
if (b != 0) L->top = (base + 2) + b;
luaD_callnoyield(L, base + 2, nresults);
adjustresults(L, nresults);
}
__jitfunc1_op7: 
puts("//opcode is : GETTABUP \n");
//opcode is : GETTABUP 
{
const TValue *slot;
int b = 0;
int c = 0;
TValue *upval = or_func->upvals[b]->v;
TValue *rc = k+c;
TString *key = tsvalue(rc);
if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {
   setobj2s(L,  base+3, slot);
} else {
   luaV_finishget(L, upval, rc,  base+3, slot);
}
}
__jitfunc1_op8: 
puts("//opcode is : LOADK    \n");
//opcode is : LOADK    
{
setobj2s(L, base + 4, k + 1);
}
__jitfunc1_op9: 
puts("//opcode is : MOVE     \n");
//opcode is : MOVE     
{
setobjs2s(L, base + 5, base + 2);
}
__jitfunc1_op10: 
puts("//opcode is : CALL     \n");
//opcode is : CALL     
{
int b = 3;
int nresults = 1 - 1;
if (b != 0) L->top = (base + 3) + b;
luaD_callnoyield(L, base + 3, nresults);
adjustresults(L, nresults);
}
__jitfunc1_op11: 
puts("//opcode is : RETURN1  \n");
//opcode is : RETURN1  
{
setobjs2s(L, L->top, base + 2);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
__jitfunc1_op12: 
puts("//opcode is : RETURN0  \n");
//opcode is : RETURN0  
return 0;
}

lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc1;
}