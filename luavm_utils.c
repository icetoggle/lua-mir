#include "luavm_utils.h"
#include <luaconf.h>
#include <ldebug.h>
#include <lgc.h>
#include <lfunc.h>
#include <lobject.h>
#include <stdio.h>

int l_strcmp (const TString *ls, const TString *rs) {
  const char *l = getstr(ls);
  size_t ll = tsslen(ls);
  const char *r = getstr(rs);
  size_t lr = tsslen(rs);
  for (;;) {  /* for each segment */
    int temp = strcoll(l, r);
    if (temp != 0)  /* not equal? */
      return temp;  /* done */
    else {  /* strings are equal up to a '\0' */
      size_t len = strlen(l);  /* index of first '\0' in both strings */
      if (len == lr)  /* 'rs' is finished? */
        return (len == ll) ? 0 : 1;  /* check 'ls' */
      else if (len == ll)  /* 'ls' is finished? */
        return -1;  /* 'ls' is less than 'rs' ('rs' is not finished) */
      /* both strings longer than 'len'; go on comparing after the '\0' */
      len++;
      l += len; ll -= len; r += len; lr -= len;
    }
  }
}

/*
** return 'l < r' for non-numbers.
*/
int lessthanothers (lua_State *L, const TValue *l, const TValue *r) {
  lua_assert(!ttisnumber(l) || !ttisnumber(r));
  if (ttisstring(l) && ttisstring(r))  /* both are strings? */
    return l_strcmp(tsvalue(l), tsvalue(r)) < 0;
  else
    return luaT_callorderTM(L, l, r, TM_LT);
}


/*
** return 'l <= r' for non-numbers.
*/
int lessequalothers (lua_State *L, const TValue *l, const TValue *r) {
  lua_assert(!ttisnumber(l) || !ttisnumber(r));
  if (ttisstring(l) && ttisstring(r))  /* both are strings? */
    return l_strcmp(tsvalue(l), tsvalue(r)) <= 0;
  else
    return luaT_callorderTM(L, l, r, TM_LE);
}

int forprep (lua_State *L, StkId ra) {
  TValue *pinit = s2v(ra);
  TValue *plimit = s2v(ra + 1);
  TValue *pstep = s2v(ra + 2);
  if (ttisinteger(pinit) && ttisinteger(pstep)) { /* integer loop? */
    lua_Integer init = ivalue(pinit);
    lua_Integer step = ivalue(pstep);
    lua_Integer limit;
    if (step == 0)
      luaG_runerror(L, "'for' step is zero");
    setivalue(s2v(ra + 3), init);  /* control variable */
    if (forlimit(L, init, plimit, &limit, step))
      return 1;  /* skip the loop */
    else {  /* prepare loop counter */
      lua_Unsigned count;
      if (step > 0) {  /* ascending loop? */
        count = l_castS2U(limit) - l_castS2U(init);
        if (step != 1)  /* avoid division in the too common case */
          count /= l_castS2U(step);
      }
      else {  /* step < 0; descending loop */
        count = l_castS2U(init) - l_castS2U(limit);
        /* 'step+1' avoids negating 'mininteger' */
        count /= l_castS2U(-(step + 1)) + 1u;
      }
      /* store the counter in place of the limit (which won't be
         needed anymore) */
      setivalue(plimit, l_castU2S(count));
    }
  }
  else {  /* try making all values floats */
    lua_Number init; lua_Number limit; lua_Number step;
    if (l_unlikely(!tonumber(plimit, &limit)))
      luaG_forerror(L, plimit, "limit");
    if (l_unlikely(!tonumber(pstep, &step)))
      luaG_forerror(L, pstep, "step");
    if (l_unlikely(!tonumber(pinit, &init)))
      luaG_forerror(L, pinit, "initial value");
    if (step == 0)
      luaG_runerror(L, "'for' step is zero");
    if (luai_numlt(0, step) ? luai_numlt(limit, init)
                            : luai_numlt(init, limit))
      return 1;  /* skip the loop */
    else {
      /* make sure internal values are all floats */
      setfltvalue(plimit, limit);
      setfltvalue(pstep, step);
      setfltvalue(s2v(ra), init);  /* internal index */
      setfltvalue(s2v(ra + 3), init);  /* control variable */
    }
  }
  return 0;
}


int forlimit (lua_State *L, lua_Integer init, const TValue *lim,
                                   lua_Integer *p, lua_Integer step) {
  if (!luaV_tointeger(lim, p, (step < 0 ? F2Iceil : F2Ifloor))) {
    /* not coercible to in integer */
    lua_Number flim;  /* try to convert to float */
    if (!tonumber(lim, &flim)) /* cannot convert to float? */
      luaG_forerror(L, lim, "limit");
    /* else 'flim' is a float out of integer bounds */
    if (luai_numlt(0, flim)) {  /* if it is positive, it is too large */
      if (step < 0) return 1;  /* initial value must be less than it */
      *p = LUA_MAXINTEGER;  /* truncate */
    }
    else {  /* it is less than min integer */
      if (step > 0) return 1;  /* initial value must be greater than it */
      *p = LUA_MININTEGER;  /* truncate */
    }
  }
  return (step > 0 ? init > *p : init < *p);  /* not to run? */
}


int floatforloop (StkId ra) {
  lua_Number step = fltvalue(s2v(ra + 2));
  lua_Number limit = fltvalue(s2v(ra + 1));
  lua_Number idx = fltvalue(s2v(ra));  /* internal index */
  idx = luai_numadd(L, idx, step);  /* increment index */
  if (luai_numlt(0, step) ? luai_numle(idx, limit)
                          : luai_numle(limit, idx)) {
    chgfltvalue(s2v(ra), idx);  /* update internal index */
    setfltvalue(s2v(ra + 3), idx);  /* and control variable */
    return 1;  /* jump back */
  }
  else
    return 0;  /* finish the loop */
}

/*
** create a new Lua closure, push it in the stack, and initialize
** its upvalues.
*/
void pushclosure (lua_State *L, Proto *p, UpVal **encup, StkId base,
                         StkId ra) {
  int nup = p->sizeupvalues;
  Upvaldesc *uv = p->upvalues;
  int i;
  LClosure *ncl = luaF_newLclosure(L, nup);
  ncl->p = p;
  setclLvalue2s(L, ra, ncl);  /* anchor new closure in stack */
  for (i = 0; i < nup; i++) {  /* fill in its upvalues */
    if (uv[i].instack)  /* upvalue refers to local variable? */
      ncl->upvals[i] = luaF_findupval(L, base + uv[i].idx);
    else  /* get upvalue from enclosing function */
      ncl->upvals[i] = encup[uv[i].idx];
    luaC_objbarrier(L, ncl, ncl->upvals[i]);
  }
}

void pushcclosure(lua_State *L, lua_CFunction func, int nup, StkId ra) {
  CClosure *ncl = luaF_newCclosure(L, nup);
  ncl->f = func;
  L->top -= nup;
  setclCvalue(L, s2v(ra), ncl);  /* anchor new closure in stack */
  while(nup--)
  {
    setobj2n(L, &ncl->upvalue[nup], s2v(L->top + nup));
    luaC_barrier(L, ncl, &ncl->upvalue[nup]);
  }
}

void print_stack(lua_State *L, StkId base) {
    int t = 20;
    printf("stack: \n");
    for (StkId sv = L->top - 1; sv >= base && t >= 0; sv--, t--) {
        const TValue *value = s2v(sv);
        switch (ttype(value))
        {
            case LUA_TNUMBER:{
                if(ttisinteger(value)){
                    printf("integer: %lld\n", ivalue(value));
                }
                else{
                    printf("float: %f\n", fltvalue(value));
                }
                break;
            }
            case LUA_TSTRING:
            {
                printf("string: %s\n", svalue(value));
                break;
            }
            case LUA_TBOOLEAN:
            {
                printf("boolean: %s\n", !l_isfalse(value) ? "true" : "false");
                break;
            }
            case LUA_TNIL:
            {
                printf("nil\n");
                break;
            }
            case LUA_TTABLE:
            {
                printf("table: %d\n", gcvalue(value));
                break;
            }
            case LUA_TFUNCTION:
            {
                printf("function: %d\n", gcvalue(value));
                break;
            }
            break;
        }
    }
    printf("stack end\n");
    printf("cli info: %lld %lld %lld\n", L->ci->func, L->ci->top, base);
}


void print_valuelist(lua_State *L, TValue *k, int ksize) {
    printf("kvalue: %lld\n", k);
    for (TValue *value = k; value != k + ksize; ++value) {
        switch (ttype(value))
        {
            case LUA_TNUMBER:{
                if(ttisinteger(value)){
                    printf("integer: %lld\n", ivalue(value));
                }
                else{
                    printf("float: %f\n", fltvalue(value));
                }
                break;
            }
            case LUA_TSTRING:
            {
                printf("string: %s %lld\n", svalue(value), gcvalue(value));
                break;
            }
            case LUA_TBOOLEAN:
            {
                printf("boolean: %s\n", !l_isfalse(value) ? "true" : "false");
                break;
            }
            case LUA_TNIL:
            {
                printf("nil %lld\n", gcvalue(value));
                break;
            }
            case LUA_TTABLE:
            {
                printf("table: %lld\n", gcvalue(value));
                break;
            }
            case LUA_TFUNCTION:
            {
                printf("function: %lld\n", gcvalue(value));
                break;
            }
            break;
        }
    }
    printf("kvalue end\n");
}


