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
__jitfunc0_op1: 
//opcode is : LOADNIL   5
updatebase(ci);
{
int ra = 2;
int b = 2;
do {
setnilvalue(s2v(base+ra++));
} while(b--);
}
__jitfunc0_op2: 
//opcode is : GETUPVAL  6
updatebase(ci);
{
int b = 0;
setobj2s(L, base+5, or_func->upvals[b]->v);
}
__jitfunc0_op3: 
//opcode is : DIVK      6
updatebase(ci);
{
TValue *v1 = s2v(base + 0);
TValue *v2 = k + 0;
lua_Number n1,n2;
if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
    setfltvalue(s2v(base + 6), luai_numdiv(L, n1, n2));
    goto __jitfunc0_op5;
}
}
__jitfunc0_op4: 
//opcode is : MMBINK    6
updatebase(ci);
{
TValue *imm = k + 0;
TMS tm = (TMS)11;
int flip = 0;
StkId result = base + 6;
luaT_trybinassocTM(L, s2v(base + 0), imm, flip, result, tm);
}
__jitfunc0_op5: 
//opcode is : CALL      6
updatebase(ci);
{
int b = 2;
int nresults = 2 - 1;
if (b != 0) L->top = (base + 5) + b;
luaD_callnoyield(L, base + 5, nresults);
adjustresults(L, nresults);
}
__jitfunc0_op6: 
//opcode is : ADDI      6
updatebase(ci);
{
TValue *v1 = s2v(base + 5);
if(ttisinteger(v1)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = 1;
    setivalue(s2v(base + 5), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op8;
}
else if (ttisfloat(v1)) {
    lua_Number n1 = fltvalue(v1);
    lua_Number n2 = cast_num(1);
    setfltvalue(s2v(base + 5), n1 + n2);
    goto __jitfunc0_op8;
}
}
__jitfunc0_op7: 
//opcode is : MMBINI    6
updatebase(ci);
{
int imm = 1;
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 5;
luaT_trybiniTM(L, s2v(base + 5), imm, flip, result, tm);
}
__jitfunc0_op8: 
//opcode is : MOVE      8
updatebase(ci);
{
setobjs2s(L, base + 6, base + 0);
}
__jitfunc0_op9: 
//opcode is : GTI       10
updatebase(ci);
{
int cond;
int im = 1;
if (ttisinteger(s2v(base + 5))) 
  cond = l_gti(ivalue(s2v(base + 5)), im);
else if (ttisfloat(s2v(base + 5))) {
  lua_Number fa = fltvalue(s2v(base + 5));
  lua_Number fim = cast_num(im);
  cond = luai_numgt(fa, fim);
} else {
  int isf = 0;
  cond = luaT_callorderiTM(L, s2v(base + 5), im, 1, isf, TM_LT);
}
if (cond != 0)
    goto __jitfunc0_op11;
else 
    goto __jitfunc0_op15;
}
__jitfunc0_op10: 
//opcode is : JMP       10
updatebase(ci);
    goto __jitfunc0_op15;
__jitfunc0_op11: 
//opcode is : ADDI      11
updatebase(ci);
{
TValue *v1 = s2v(base + 5);
if(ttisinteger(v1)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = -1;
    setivalue(s2v(base + 5), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op13;
}
else if (ttisfloat(v1)) {
    lua_Number n1 = fltvalue(v1);
    lua_Number n2 = cast_num(-1);
    setfltvalue(s2v(base + 5), n1 + n2);
    goto __jitfunc0_op13;
}
}
__jitfunc0_op12: 
//opcode is : MMBINI    11
updatebase(ci);
{
int imm = 1;
TMS tm = (TMS)7;
int flip = 0;
StkId result = base + 5;
luaT_trybiniTM(L, s2v(base + 5), imm, flip, result, tm);
}
__jitfunc0_op13: 
//opcode is : GETTABLE  12
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+1);
TValue *rc = s2v(base+5);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 4, slot);
} else {
   luaV_finishget(L, rb, rc,  base+4, slot);
}
}
__jitfunc0_op14: 
//opcode is : JMP       12
updatebase(ci);
    goto __jitfunc0_op24;
__jitfunc0_op15: 
//opcode is : GETTABLE  14
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+1);
TValue *rc = s2v(base+6);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 4, slot);
} else {
   luaV_finishget(L, rb, rc,  base+4, slot);
}
}
__jitfunc0_op16: 
//opcode is : GETI      15
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+1);
if (luaV_fastgeti(L, rb, 1, slot)) {
   setobj2s(L, base+7, slot);
} else {
   TValue key;
   setivalue(&key, 1);
   luaV_finishget(L, rb, &key,  base+7, slot);
}
}
__jitfunc0_op17: 
//opcode is : SETTABLE  15
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base + 6); 
TValue *rc = s2v(base+7);
lua_Unsigned n;
if (ttisinteger(rb)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rb)), luaV_fastgeti(L, s2v(base + 1), n, slot))
    : luaV_fastget(L, s2v(base + 1), rb, slot, luaH_get)) {
   luaV_finishfastset(L, s2v(base + 1), slot, rc);
} else {
   luaV_finishset(L, s2v(base + 1), rb, rc, slot);
}
}
__jitfunc0_op18: 
//opcode is : ADDI      16
updatebase(ci);
{
TValue *v1 = s2v(base + 6);
if(ttisinteger(v1)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = -1;
    setivalue(s2v(base + 6), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op20;
}
else if (ttisfloat(v1)) {
    lua_Number n1 = fltvalue(v1);
    lua_Number n2 = cast_num(-1);
    setfltvalue(s2v(base + 6), n1 + n2);
    goto __jitfunc0_op20;
}
}
__jitfunc0_op19: 
//opcode is : MMBINI    16
updatebase(ci);
{
int imm = 1;
TMS tm = (TMS)7;
int flip = 0;
StkId result = base + 6;
luaT_trybiniTM(L, s2v(base + 6), imm, flip, result, tm);
}
__jitfunc0_op20: 
//opcode is : EQI       17
updatebase(ci);
{
int cond;
int im = 1;
if (ttisinteger(s2v(base + 6)))
 cond = (ivalue(s2v(base + 6)) == im);
else if (ttisfloat(s2v(base + 6)))
 cond = luai_numeq(fltvalue(s2v(base + 6)), cast_num(im));
else
 cond = 0;
if (cond != 0) {
    goto __jitfunc0_op22;
} else {
    goto __jitfunc0_op24;
}
}
__jitfunc0_op21: 
//opcode is : JMP       17
updatebase(ci);
    goto __jitfunc0_op24;
__jitfunc0_op22: 
//opcode is : SETI      18
updatebase(ci);
{
const TValue *slot;
int c = 1;
TValue *rc = s2v(base+4);
if (luaV_fastgeti(L, s2v(base+1), c, slot)) {
   luaV_finishfastset(L, s2v(base+1), slot, rc);
} else {
   TValue key;
   setivalue(&key, c);
   luaV_finishset(L, s2v(base+1), &key, rc, slot);
}
}
__jitfunc0_op23: 
//opcode is : RETURN0   19
updatebase(ci);
return 0;
__jitfunc0_op24: 
//opcode is : MOVE      22
updatebase(ci);
{
setobjs2s(L, base + 3, base + 5);
}
__jitfunc0_op25: 
//opcode is : MULK      23
updatebase(ci);
{
TValue *v1 = s2v(base + 5);
TValue *v2 = k + 0;
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 2), l_castU2S(l_castS2U(i1) * l_castS2U(i2)));
    goto __jitfunc0_op27;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
        setfltvalue(s2v(base + 2), n1 * n2);
    goto __jitfunc0_op27;
    }
}
}
__jitfunc0_op26: 
//opcode is : MMBINK    23
updatebase(ci);
{
TValue *imm = k + 0;
TMS tm = (TMS)8;
int flip = 0;
StkId result = base + 2;
luaT_trybinassocTM(L, s2v(base + 5), imm, flip, result, tm);
}
__jitfunc0_op27: 
//opcode is : LE        24
updatebase(ci);
{
int cond;
TValue *rb = s2v(base+6);
if (ttisinteger(s2v(base+2)) && ttisinteger(rb)) {
  lua_Integer ia = ivalue(s2v(base+2));
  lua_Integer ib = ivalue(rb);
  cond = l_lei(ia,ib);
} else if (ttisnumber(s2v(base+2)) && ttisnumber(rb)) {
  cond = LEnum(s2v(base+2), rb); 
} else {
  cond = lessequalothers(L, s2v(base+2), rb);
}
if (cond != 0)
    goto __jitfunc0_op29;
else 
    goto __jitfunc0_op51;
}
__jitfunc0_op28: 
//opcode is : JMP       24
updatebase(ci);
    goto __jitfunc0_op51;
__jitfunc0_op29: 
//opcode is : LT        25
updatebase(ci);
{
int cond;
TValue *rb = s2v(base+6);
if (ttisinteger(s2v(base+2)) && ttisinteger(rb)) {
  lua_Integer ia = ivalue(s2v(base+2));
  lua_Integer ib = ivalue(rb);
  cond = l_lti(ia,ib);
} else if (ttisnumber(s2v(base+2)) && ttisnumber(rb)) {
  cond = LTnum(s2v(base+2), rb); 
} else {
  cond = lessthanothers(L, s2v(base+2), rb);
}
if (cond != 0)
    goto __jitfunc0_op31;
else 
    goto __jitfunc0_op39;
}
__jitfunc0_op30: 
//opcode is : JMP       25
updatebase(ci);
    goto __jitfunc0_op39;
__jitfunc0_op31: 
//opcode is : GETTABLE  25
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+1);
TValue *rc = s2v(base+2);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 7, slot);
} else {
   luaV_finishget(L, rb, rc,  base+7, slot);
}
}
__jitfunc0_op32: 
//opcode is : ADDI      25
updatebase(ci);
{
TValue *v1 = s2v(base + 2);
if(ttisinteger(v1)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = 1;
    setivalue(s2v(base + 8), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op34;
}
else if (ttisfloat(v1)) {
    lua_Number n1 = fltvalue(v1);
    lua_Number n2 = cast_num(1);
    setfltvalue(s2v(base + 8), n1 + n2);
    goto __jitfunc0_op34;
}
}
__jitfunc0_op33: 
//opcode is : MMBINI    25
updatebase(ci);
{
int imm = 1;
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 8;
luaT_trybiniTM(L, s2v(base + 2), imm, flip, result, tm);
}
__jitfunc0_op34: 
//opcode is : GETTABLE  25
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+1);
TValue *rc = s2v(base+8);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 8, slot);
} else {
   luaV_finishget(L, rb, rc,  base+8, slot);
}
}
__jitfunc0_op35: 
//opcode is : LT        25
updatebase(ci);
{
int cond;
TValue *rb = s2v(base+8);
if (ttisinteger(s2v(base+7)) && ttisinteger(rb)) {
  lua_Integer ia = ivalue(s2v(base+7));
  lua_Integer ib = ivalue(rb);
  cond = l_lti(ia,ib);
} else if (ttisnumber(s2v(base+7)) && ttisnumber(rb)) {
  cond = LTnum(s2v(base+7), rb); 
} else {
  cond = lessthanothers(L, s2v(base+7), rb);
}
if (cond != 0)
    goto __jitfunc0_op37;
else 
    goto __jitfunc0_op39;
}
__jitfunc0_op36: 
//opcode is : JMP       25
updatebase(ci);
    goto __jitfunc0_op39;
__jitfunc0_op37: 
//opcode is : ADDI      26
updatebase(ci);
{
TValue *v1 = s2v(base + 2);
if(ttisinteger(v1)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = 1;
    setivalue(s2v(base + 2), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op39;
}
else if (ttisfloat(v1)) {
    lua_Number n1 = fltvalue(v1);
    lua_Number n2 = cast_num(1);
    setfltvalue(s2v(base + 2), n1 + n2);
    goto __jitfunc0_op39;
}
}
__jitfunc0_op38: 
//opcode is : MMBINI    26
updatebase(ci);
{
int imm = 1;
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 2;
luaT_trybiniTM(L, s2v(base + 2), imm, flip, result, tm);
}
__jitfunc0_op39: 
//opcode is : GETTABLE  28
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+1);
TValue *rc = s2v(base+2);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 7, slot);
} else {
   luaV_finishget(L, rb, rc,  base+7, slot);
}
}
__jitfunc0_op40: 
//opcode is : LT        28
updatebase(ci);
{
int cond;
TValue *rb = s2v(base+7);
if (ttisinteger(s2v(base+4)) && ttisinteger(rb)) {
  lua_Integer ia = ivalue(s2v(base+4));
  lua_Integer ib = ivalue(rb);
  cond = l_lti(ia,ib);
} else if (ttisnumber(s2v(base+4)) && ttisnumber(rb)) {
  cond = LTnum(s2v(base+4), rb); 
} else {
  cond = lessthanothers(L, s2v(base+4), rb);
}
if (cond != 0)
    goto __jitfunc0_op42;
else 
    goto __jitfunc0_op48;
}
__jitfunc0_op41: 
//opcode is : JMP       28
updatebase(ci);
    goto __jitfunc0_op48;
__jitfunc0_op42: 
//opcode is : GETTABLE  29
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base+1);
TValue *rc = s2v(base+2);
lua_Unsigned n;
if (ttisinteger(rc)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rc)), luaV_fastgeti(L, rb, n, slot))
    : luaV_fastget(L, rb, rc, slot, luaH_get)) {
   setobj2s(L, base + 7, slot);
} else {
   luaV_finishget(L, rb, rc,  base+7, slot);
}
}
__jitfunc0_op43: 
//opcode is : SETTABLE  29
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base + 3); 
TValue *rc = s2v(base+7);
lua_Unsigned n;
if (ttisinteger(rb)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rb)), luaV_fastgeti(L, s2v(base + 1), n, slot))
    : luaV_fastget(L, s2v(base + 1), rb, slot, luaH_get)) {
   luaV_finishfastset(L, s2v(base + 1), slot, rc);
} else {
   luaV_finishset(L, s2v(base + 1), rb, rc, slot);
}
}
__jitfunc0_op44: 
//opcode is : MOVE      30
updatebase(ci);
{
setobjs2s(L, base + 3, base + 2);
}
__jitfunc0_op45: 
//opcode is : ADD       31
updatebase(ci);
{
TValue *v1 = s2v(base + 2);
TValue *v2 = s2v(base + 3);
if(ttisinteger(v1) && ttisinteger(v2)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = ivalue(v2);
    setivalue(s2v(base + 2), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op47;
} else {
    lua_Number n1,n2;
    if (tonumberns(v1, n1) && tonumberns(v2, n2)) {
       setfltvalue(s2v(base + 2), n1 + n2);
    goto __jitfunc0_op47;
    }
}
}
__jitfunc0_op46: 
//opcode is : MMBIN     31
updatebase(ci);
{
TValue *rb = s2v(base + 3);
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 2;
luaT_trybinTM(L, s2v(base + 2), rb, result, tm);
}
__jitfunc0_op47: 
//opcode is : JMP       31
updatebase(ci);
    goto __jitfunc0_op27;
__jitfunc0_op48: 
//opcode is : ADDI      33
updatebase(ci);
{
TValue *v1 = s2v(base + 6);
if(ttisinteger(v1)) {
    lua_Integer i1 = ivalue(v1);
    lua_Integer i2 = 1;
    setivalue(s2v(base + 2), l_castU2S(l_castS2U(i1) + l_castS2U(i2)));
    goto __jitfunc0_op50;
}
else if (ttisfloat(v1)) {
    lua_Number n1 = fltvalue(v1);
    lua_Number n2 = cast_num(1);
    setfltvalue(s2v(base + 2), n1 + n2);
    goto __jitfunc0_op50;
}
}
__jitfunc0_op49: 
//opcode is : MMBINI    33
updatebase(ci);
{
int imm = 1;
TMS tm = (TMS)6;
int flip = 0;
StkId result = base + 2;
luaT_trybiniTM(L, s2v(base + 6), imm, flip, result, tm);
}
__jitfunc0_op50: 
//opcode is : JMP       34
updatebase(ci);
    goto __jitfunc0_op27;
__jitfunc0_op51: 
//opcode is : SETTABLE  36
updatebase(ci);
{
const TValue *slot;
TValue *rb = s2v(base + 3); 
TValue *rc = s2v(base+4);
lua_Unsigned n;
if (ttisinteger(rb)  /* fast track for integers? */
    ? (cast_void(n = ivalue(rb)), luaV_fastgeti(L, s2v(base + 1), n, slot))
    : luaV_fastget(L, s2v(base + 1), rb, slot, luaH_get)) {
   luaV_finishfastset(L, s2v(base + 1), slot, rc);
} else {
   luaV_finishset(L, s2v(base + 1), rb, rc, slot);
}
}
__jitfunc0_op52: 
//opcode is : JMP       36
updatebase(ci);
    goto __jitfunc0_op9;
__jitfunc0_op53: 
//opcode is : RETURN0   38
updatebase(ci);
return 0;
return 0;
}
lua_CFunction create_test_func(lua_State *L)
{
    return __jit_lfunc0;
}