#include "c2cluafunc.h"
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
#include "mir_utils.h"

struct JitCode {
    char *code;
    size_t code_size;
    size_t cur_idx; 
};



static int get_data(void *data)
{
    struct JitCode *code = (struct JitCode *)data;
    if (code->cur_idx >= code->code_size) {
        return EOF;
    }
    return code->code[code->cur_idx++];
}






lua_CFunction create_clua_func_from_c(lua_State *L, const char *source_name, const char* code)
{
    mir_ctx_wrapper* wrapper = init_mir_context(L);
    MIR_context_t ctx = wrapper->ctx;
    MIR_gen_init(ctx, 2);
    c2mir_init(ctx);
    struct JitCode code_data;
    code_data.code = (char *)code;
    code_data.code_size = strlen(code);
    code_data.cur_idx = 0;
    wrapper->options.module_num++;
    if(!c2mir_compile(ctx, &wrapper->options, get_data, &code_data, source_name, NULL)) {
        puts(code);
        luaL_error(L, "c2mir_compile failed");
        return NULL;
    }
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
    // MIR_gen(ctx, 0, rt_func);
    MIR_gen_finish(ctx);
    c2mir_finish(ctx);
    lua_CFunction lua_func = (lua_CFunction)rt_func->addr;
    return lua_func;
}