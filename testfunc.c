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
#define savestate(L,ci)		(L->top = ci->top)
#define Protect(exp)  (savestate(L,ci), (exp))
static int __jit_lfunc0(lua_State *L) {
    CallInfo *ci = L->ci;
    StkId base = ci->func + 1;
    for(StkId skd = L->top; skd <= base; ++skd) {
       setnilvalue(s2v(skd));
    }
     CClosure *func = clCvalue(s2v(ci->func));
Table *ktable = luaH_new(L);
{
luaH_resize(L, ktable, 4, 0);
TValue h;
sethvalue(L, &h, ktable);
TValue key;
TValue value;
const TValue* slot;
TString* ts;
slot = NULL;
setivalue(&key, 1);
ts = luaS_new(L, "print");
setsvalue(L, &value, ts);
if (luaV_fastgeti(L, &h, 1, slot)) {
   luaV_finishfastset(L, &h, slot, &value);
}
else {
  luaV_finishset(L, &h, &key, &value, slot);
}
slot = NULL;
setivalue(&key, 2);
ts = luaS_new(L, "isplaceokinner");
setsvalue(L, &value, ts);
if (luaV_fastgeti(L, &h, 2, slot)) {
   luaV_finishfastset(L, &h, slot, &value);
}
else {
  luaV_finishset(L, &h, &key, &value, slot);
}
slot = NULL;
setivalue(&key, 3);
ts = luaS_new(L, "isplaceokinner3");
setsvalue(L, &value, ts);
if (luaV_fastgeti(L, &h, 3, slot)) {
   luaV_finishfastset(L, &h, slot, &value);
}
else {
  luaV_finishset(L, &h, &key, &value, slot);
}
slot = NULL;
setivalue(&key, 4);
ts = luaS_new(L, "isplaceokinner2");
setsvalue(L, &value, ts);
if (luaV_fastgeti(L, &h, 4, slot)) {
   luaV_finishfastset(L, &h, slot, &value);
}
else {
  luaV_finishset(L, &h, &key, &value, slot);
}
setobj2n(L, &func->upvalue[1], &h);
}
     TValue *k = ktable->array;
//opcode is : GETTABUP  7
updatebase(ci);
{
const TValue *slot;
int b = 0;
int c = 0;
TValue *upval = func->upvalue + b;
TValue *rc = k+c;
TString *key = tsvalue(rc);
if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {
   setobj2s(L,  base+3, slot);
} else {
   luaV_finishget(L, upval, rc,  base+3, slot);
}
}
//opcode is : LOADK     7
updatebase(ci);
{
setobj2s(L, base + 4, k + 1);
printf("k value %s\n", svalue(s2v(base + 4)));
}
//opcode is : CALL      7
updatebase(ci);
{
int b = 2;
int nresults = 1 - 1;
if (b != 0) L->top = (base + 3) + b;
luaD_callnoyield(L, base + 3, nresults);
adjustresults(L, nresults);
}
//opcode is : LOADI     8
updatebase(ci);
{
lua_Integer b = 1;
setivalue(s2v(base+3), b);
}
//opcode is : ADDI      8
updatebase(ci);
{
TValue *v1 = s2v(base + 1);
if(ttisinteger(v1)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = -1;
    setivalue(s2v(base + 4), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op7;
}
else if (ttisfloat(v1)) {
    lua_Number n1 = fltvalue(v1);
    lua_Number n2 = cast_num(-1);
    setfltvalue(s2v(base + 4), n1 + n2);
    goto __jitfunc0_op7;
}
}
//opcode is : MMBINI    8
updatebase(ci);
{
int imm = 1;
TMS tm = (TMS)7;
int flip = 0;
StkId result = base + 4;
luaT_trybiniTM(L, s2v(base + 1), imm, flip, result, tm);
}
__jitfunc0_op7: 
//opcode is : LOADI     8
updatebase(ci);
{
lua_Integer b = 1;
setivalue(s2v(base+5), b);
}
//opcode is : FORPREP   8
updatebase(ci);
{
if (forprep(L, base + 3))
    goto __jitfunc0_op32;
}
__jitfunc0_op9: 
//opcode is : GETTABLE  9
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+0);
TValue *rc = s2v(base+6);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 7, slot);
} else {
   luaV_finishget(L, rb, rc,  base+7, slot);
}
}
//opcode is : EQ        9
updatebase(ci);
{
int cond;
TValue *rb = s2v(base + 2);
cond = luaV_equalobj(L, s2v(base + 7), rb);
if (cond != 1) {
    goto __jitfunc0_op12;
} else {
    goto __jitfunc0_op26;
}
}
//opcode is : JMP       9
updatebase(ci);
    goto __jitfunc0_op26;
__jitfunc0_op12: 
//opcode is : GETTABLE  10
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+0);
TValue *rc = s2v(base+6);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 7, slot);
} else {
   luaV_finishget(L, rb, rc,  base+7, slot);
}
}
//opcode is : SUB       10
updatebase(ci);
{
TValue *v1 = s2v(base + 7);
TValue *v2 = s2v(base + 6);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 7), l_castU2S(l_castS2U(i1) - l_castS2U(i2)));
    goto __jitfunc0_op15;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 7), n1 - n2);
    goto __jitfunc0_op15;
    }
}
}
//opcode is : MMBIN     10
updatebase(ci);
{
TValue *rb = s2v(base + 6);
TMS tm = (TMS)7;
int flip = 0;
StkId result = base + 7;
luaT_trybinTM(L, s2v(base + 7), rb, result, tm);
}
__jitfunc0_op15: 
//opcode is : SUB       10
updatebase(ci);
{
TValue *v1 = s2v(base + 2);
TValue *v2 = s2v(base + 1);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 8), l_castU2S(l_castS2U(i1) - l_castS2U(i2)));
    goto __jitfunc0_op17;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 8), n1 - n2);
    goto __jitfunc0_op17;
    }
}
}
//opcode is : MMBIN     10
updatebase(ci);
{
TValue *rb = s2v(base + 1);
TMS tm = (TMS)7;
int flip = 0;
StkId result = base + 8;
luaT_trybinTM(L, s2v(base + 2), rb, result, tm);
}
__jitfunc0_op17: 
//opcode is : EQ        10
updatebase(ci);
{
int cond;
TValue *rb = s2v(base + 8);
cond = luaV_equalobj(L, s2v(base + 7), rb);
if (cond != 1) {
    goto __jitfunc0_op19;
} else {
    goto __jitfunc0_op26;
}
}
//opcode is : JMP       10
updatebase(ci);
    goto __jitfunc0_op26;
__jitfunc0_op19: 
//opcode is : GETTABLE  11
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+0);
TValue *rc = s2v(base+6);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 7, slot);
} else {
   luaV_finishget(L, rb, rc,  base+7, slot);
}
}
//opcode is : ADD       11
updatebase(ci);
{
TValue *v1 = s2v(base + 7);
TValue *v2 = s2v(base + 6);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 7), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op22;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 7), n1 + n2);
    goto __jitfunc0_op22;
    }
}
}
//opcode is : MMBIN     11
updatebase(ci);
{
TValue *rb = s2v(base + 6);
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 7;
luaT_trybinTM(L, s2v(base + 7), rb, result, tm);
}
__jitfunc0_op22: 
//opcode is : ADD       11
updatebase(ci);
{
TValue *v1 = s2v(base + 2);
TValue *v2 = s2v(base + 1);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 8), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op24;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 8), n1 + n2);
    goto __jitfunc0_op24;
    }
}
}
//opcode is : MMBIN     11
updatebase(ci);
{
TValue *rb = s2v(base + 1);
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 8;
luaT_trybinTM(L, s2v(base + 2), rb, result, tm);
}
__jitfunc0_op24: 
//opcode is : EQ        11
updatebase(ci);
{
int cond;
TValue *rb = s2v(base + 8);
cond = luaV_equalobj(L, s2v(base + 7), rb);
if (cond != 0) {
    goto __jitfunc0_op26;
} else {
    goto __jitfunc0_op31;
}
}
//opcode is : JMP       11
updatebase(ci);
    goto __jitfunc0_op31;
__jitfunc0_op26: 
//opcode is : GETTABUP  12
updatebase(ci);
{
const TValue *slot;
int b = 0;
int c = 0;
TValue *upval = func->upvalue + b;
TValue *rc = k+c;
TString *key = tsvalue(rc);
if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {
   setobj2s(L,  base+7, slot);
} else {
   luaV_finishget(L, upval, rc,  base+7, slot);
}
}
//opcode is : LOADK     12
updatebase(ci);
{
setobj2s(L, base + 8, k + 2);
printf("k value %s\n", svalue(s2v(base + 8)));
}
//opcode is : CALL      12
updatebase(ci);
{
int b = 2;
int nresults = 1 - 1;
if (b != 0) L->top = (base + 7) + b;
luaD_callnoyield(L, base + 7, nresults);
adjustresults(L, nresults);
}
//opcode is : LOADFALSE 13
updatebase(ci);
setbfvalue(s2v(base+7));
//opcode is : RETURN1   13
updatebase(ci);
{
setobjs2s(L, L->top, base + 7);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
__jitfunc0_op31: 
//opcode is : FORLOOP   8
updatebase(ci);
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
    goto __jitfunc0_op9;
  }
}
else if (floatforloop(base + 3))  /* float loop */
    goto __jitfunc0_op9;
}
__jitfunc0_op32: 
//opcode is : GETTABUP  16
updatebase(ci);
{
const TValue *slot;
int b = 0;
int c = 0;
TValue *upval = func->upvalue + b;
TValue *rc = k+c;
TString *key = tsvalue(rc);
if (luaV_fastget(L, upval, key, slot, luaH_getshortstr)) {
   setobj2s(L,  base+3, slot);
} else {
   luaV_finishget(L, upval, rc,  base+3, slot);
}
}
//opcode is : LOADK     16
updatebase(ci);
{
setobj2s(L, base + 4, k + 3);
printf("k value %s\n", svalue(s2v(base + 4)));
}
//opcode is : CALL      16
updatebase(ci);
{
int b = 2;
int nresults = 1 - 1;
if (b != 0) L->top = (base + 3) + b;
luaD_callnoyield(L, base + 3, nresults);
adjustresults(L, nresults);
}
//opcode is : LOADTRUE  17
updatebase(ci);
setbtvalue(s2v(base+3));
//opcode is : RETURN1   17
updatebase(ci);
{
setobjs2s(L, L->top, base + 3);
api_incr_top(L);
luaC_checkGC(L);
return 1;
}
//opcode is : RETURN0   18
updatebase(ci);
return 0;
return 0;
}
lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc0;
}