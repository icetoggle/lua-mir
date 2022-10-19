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
static int __jit_lfunc0(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    for(StkId skd = L->top; skd <= base; ++skd) {
       setnilvalue(s2v(skd));
    }
     CClosure *func = clCvalue(s2v(ci->func));
     LClosure *or_func = clLvalue(func->upvalue);
     TValue *k = or_func->p->k;
__jitfunc0_op0: 
//opcode is : LOADI    
{
lua_Integer b = 1;
setivalue(s2v(base+1), b);
}
__jitfunc0_op1: 
//opcode is : LOADI    
{
lua_Integer b = 1;
setivalue(s2v(base+2), b);
}
__jitfunc0_op2: 
//opcode is : LOADI    
{
lua_Integer b = 1;
setivalue(s2v(base+3), b);
}
__jitfunc0_op3: 
//opcode is : MOVE     
{
setobjs2s(L, base + 4, base + 0);
}
__jitfunc0_op4: 
//opcode is : LOADI    
{
lua_Integer b = 1;
setivalue(s2v(base+5), b);
}
__jitfunc0_op5: 
//opcode is : FORPREP  
{
if (forprep(L, base + 3))
    goto __jitfunc0_op13;
}
__jitfunc0_op6: 
//opcode is : MOVE     
{
setobjs2s(L, base + 7, base + 2);
}
__jitfunc0_op7: 
//opcode is : ADD      
{
TValue *v1 = s2v(base + 1);
TValue *v2 = s2v(base + 2);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 8), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op9;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 8), n1 + n2);
    goto __jitfunc0_op9;
    }
}
}
__jitfunc0_op8: 
//opcode is : MMBIN    
{
TValue *rb = s2v(base + 2);
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 8;
luaT_trybinTM(L, s2v(base + 1), rb, result, tm);
}
__jitfunc0_op9: 
//opcode is : MODK     
{
TValue *v1 = s2v(base + 8);
TValue *v2 = k + 0;
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 2), luaV_mod(L, i1, i2));
    goto __jitfunc0_op11;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
        setfltvalue(s2v(base + 2), luaV_modf(L, n1, n2));
    goto __jitfunc0_op11;
    }
}
}
__jitfunc0_op10: 
//opcode is : MMBINK   
{
TValue *imm = k + 0;
TMS tm = (TMS)9;
int flip = 0;
StkId result = base + 2;
luaT_trybinassocTM(L, s2v(base + 8), imm, flip, result, tm);
}
__jitfunc0_op11: 
//opcode is : MOVE     
{
setobjs2s(L, base + 1, base + 7);
}
__jitfunc0_op12: 
//opcode is : FORLOOP  
{
if (ttisinteger(s2v(base + 3 + 2))) {  /* integer loop? */
  lua_Unsigned count = l_castS2U(ivalue(s2v(base + 3 + 1)));
  if (count > 0) {  /* still more iterations? */
    lua_Integer step = ivalue(s2v(base + 3 + 2));
    lua_Integer idx = ivalue(s2v(base + 3));  /* internal index */
    chgivalue(s2v(base + 3 + 1), count - 1);  /* update counter */
    idx = intop(+, idx, step);  /* add step to index */
    chgivalue(s2v(base + 3), idx);  /* update internal index */
    setivalue(s2v(base + 3 + 3), idx);  /* and control variable */
    goto __jitfunc0_op6;
  }
}
else if (floatforloop(base + 3))  /* float loop */
    goto __jitfunc0_op6;
}
__jitfunc0_op13: 
//opcode is : RETURN1  
{
setobjs2s(L, L->top, base + 2);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
__jitfunc0_op14: 
//opcode is : RETURN0  
return 0;
}
lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc0;
}