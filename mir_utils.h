#ifndef MIR_UTILS_H
#define MIR_UTILS_H
#include <mir.h>
#include <lua.h>
#include <c2mir/c2mir.h>
typedef struct {
    MIR_context_t ctx;
    struct c2mir_options options;
}mir_ctx_wrapper;


mir_ctx_wrapper* init_mir_context(lua_State *L);
#endif