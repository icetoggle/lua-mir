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
static int __jit_lfunc4(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    for(StkId skd = L->top; skd <= base; ++skd) {
       setnilvalue(s2v(skd));
    }
     CClosure *func = clCvalue(s2v(ci->func));
     LClosure *or_func = clLvalue(func->upvalue);
     TValue *k = or_func->p->k;
__jitfunc4_op0: 
//opcode is : VARARGPREP
{
luaT_adjustvarargs(L, 0, ci, or_func->p);
base = ci->func + 1;
}
__jitfunc4_op1: 
//opcode is : LOADI    
{
lua_Integer b = 0;
setivalue(s2v(base+0), b);
}
__jitfunc4_op2: 
//opcode is : GETTABUP 
{
const TValue *slot;
int b = 0;
int c = 0;
TValue *upval = or_func->upvals[b]->v;
TValue *rc = k+c;
TString *key = tsvalue(rc);
if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {
   setobj2s(L,  base+1, slot);
} else {
   luaV_finishget(L, upval, rc,  base+1, slot);
}
}
__jitfunc4_op3: 
//opcode is : NEWTABLE 
{
int b = 0;
int c = 0;
Table *t;
if (b > 0)
  b = 1 << (b - 1);  /* size is 2^(b - 1) */
L->top = base + 2 + 1;    /* correct top in case of emergency GC */
t = luaH_new(L);  /* memory allocation */
sethvalue2s(L, base + 2, t);
if (b != 0 || c != 0)
  luaH_resize(L, t, c, b);  /* idem */
luaC_checkGC(L);
    goto __jitfunc4_op5;
}
__jitfunc4_op5: 
//opcode is : VARARG   
{
int n = 0 - 1;  /* required results */
luaT_getvarargs(L, ci, base + 3, n);
}
__jitfunc4_op6: 
//opcode is : SETLIST  
{
int n = 0;
unsigned int last = 0;
Table *h = hvalue(s2v(base + 2));
if (n == 0) 
  n = cast_int(L->top - (base + 2)) - 1;
else
  L->top = ci->top;
last += n;
if (last > luaH_realasize(h))  /* needs more space? */
  luaH_resizearray(L, h, last);  /* preallocate it at once */for (; n > 0; n--) {
  TValue *val = s2v(base + 2 + n);
  setobj2t(L, &h->array[last - 1], val);
  last--;
  luaC_barrierback(L, h, val);
}
}
__jitfunc4_op7: 
//opcode is : CALL     
{
int b = 2;
int nresults = 5 - 1;
if (b != 0) L->top = (base + 1) + b;
luaD_callnoyield(L, base + 1, nresults);
adjustresults(L, nresults);
}
__jitfunc4_op8: 
//opcode is : TFORPREP 
{
luaF_newtbcupval(L, base + 1 + 3);
    goto __jitfunc4_op11;
}
__jitfunc4_op9: 
//opcode is : ADD      
{
TValue *v1 = s2v(base + 0);
TValue *v2 = s2v(base + 6);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 0), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc4_op11;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 0), n1 + n2);
    goto __jitfunc4_op11;
    }
}
}
__jitfunc4_op10: 
//opcode is : MMBIN    
{
TValue *rb = s2v(base + 6);
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 0;
luaT_trybinTM(L, s2v(base + 0), rb, result, tm);
}
__jitfunc4_op11: 
//opcode is : TFORCALL 
{
StkId ra = base + 1;
memcpy(ra + 4, ra, 3 * sizeof(*ra));
L->top = ra + 4 + 3;
luaD_callnoyield(L, ra + 4, 2);
}
__jitfunc4_op12: 
//opcode is : TFORLOOP 
{
StkId ra = base + 1;
if (!ttisnil(s2v(ra + 4))) {  /* continue loop? */
  setobjs2s(L, ra + 2, ra + 4);  /* save control variable */
    goto __jitfunc4_op9;
}
}
__jitfunc4_op13: 
//opcode is : CLOSE    
luaF_close(L, base + 1, LUA_OK, 1);
__jitfunc4_op14: 
//opcode is : RETURN   
{
int n = 2 - 1;
if (n < 0) 
  n = cast_int(L->top - (base + 0));
for(int i = 0; i < n; ++i) {
  setobj2s(L, L->top, base + 0 + i);
  api_incr_top(L);
}
return n;
}
__jitfunc4_op15: 
//opcode is : RETURN   
{
int n = 1 - 1;
if (n < 0) 
  n = cast_int(L->top - (base + 1));
for(int i = 0; i < n; ++i) {
  setobj2s(L, L->top, base + 1 + i);
  api_incr_top(L);
}
return n;
}
return 0;
}

lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc4;
}