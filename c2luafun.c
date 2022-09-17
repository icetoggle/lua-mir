#include "c2luafun.h"
#include <stdio.h>
#include <string.h>
#include <mir.h>
#include <mir-dlist.h>
#include <mir-gen.h>
#include <c2mir/c2mir.h>
#include <mir-varr.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "lua_std_cfunc.h"

struct JitCode {
    char *code;
    size_t code_size;
    size_t cur_idx; 
};

typedef const char *char_ptr_t;

DEF_VARR (char_ptr_t);


static int get_data(void *data)
{
    struct JitCode *code = (struct JitCode *)data;
    if (code->cur_idx >= code->code_size) {
        return EOF;
    }
    return code->code[code->cur_idx++];
}

typedef struct {
    MIR_context_t ctx;
}mir_ctx_wrapper;

static int mir_context_gc(lua_State *L)
{
    mir_ctx_wrapper *ctx_wrapper = (mir_ctx_wrapper *)lua_touserdata(L, 1);
    MIR_finish(ctx_wrapper->ctx);
    return 0;
}

static MIR_context_t init_mir_context(lua_State *L)
{
    lua_pushstring(L, "__mir_context");
    lua_gettable(L, LUA_REGISTRYINDEX);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        MIR_context_t ctx = MIR_init();
        lua_pushstring(L, "__mir_context");
        mir_ctx_wrapper *wrapper = lua_newuserdata(L, sizeof(mir_ctx_wrapper));
        wrapper->ctx = ctx;
        luaL_newmetatable(L, "mir_context");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, mir_context_gc);
        lua_settable(L, -3);
        lua_setmetatable(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);
        return ctx;
    } else {
        MIR_context_t ctx = lua_touserdata(L, -1);
        lua_pop(L, 1);
        return ctx;
    }
    return NULL;
}


lua_CFunction create_lua_func_from_c(lua_State *L, const char *source_name, const char* code)
{
    MIR_context_t ctx = init_mir_context(L);
    MIR_gen_init(ctx, 1);
    c2mir_init(ctx);
    struct c2mir_options options;
    memset(&options, 0, sizeof(options));
    VARR(char_ptr_t) *headers;
    VARR_CREATE(char_ptr_t, headers, 0);
    VARR_PUSH(char_ptr_t, headers, "/usr/local/include");
    options.include_dirs = VARR_ADDR(char_ptr_t, headers);
    options.include_dirs_num = 1;
    struct JitCode code_data;
    code_data.code = (char *)code;
    code_data.code_size = strlen(code);
    code_data.cur_idx = 0;
    if(!c2mir_compile(ctx, &options, get_data, &code_data, source_name, NULL)) {
        luaL_error(L, "c2mir_compile failed");
        return NULL;
    }
    c2mir_finish(ctx);
    MIR_module_t module;
    MIR_item_t func = NULL;
    MIR_item_t rt_func = NULL;
    for(module = DLIST_HEAD(MIR_module_t, *MIR_get_module_list(ctx)); module; module = DLIST_NEXT(MIR_module_t, module)) {
        for(func = DLIST_HEAD(MIR_item_t, module->items); func; func = DLIST_NEXT(MIR_item_t, func)) {
            if (func->item_type == MIR_func_item) {
                if (strcmp(func->u.func->name, source_name) == 0) {
                    rt_func = func;
                }
            }
        }
        MIR_load_module(ctx, module);
    }
    MIR_gen_set_optimize_level(ctx, 0, 3);
    if(rt_func == NULL)
    {
        luaL_error(L, "rt_func == NULL");
        return NULL;
    }
    MIR_link(ctx, MIR_set_gen_interface, import_luacfun_resolver);
    MIR_gen(ctx, 0, rt_func);
    MIR_gen_finish(ctx);
    lua_CFunction lua_func = (lua_CFunction)rt_func->addr;
    return lua_func;
}