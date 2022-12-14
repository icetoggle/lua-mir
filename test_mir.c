#include <stdio.h>
#include <string.h>
#include <mir.h>
#include <mir-dlist.h>
#include <mir-gen.h>
#include <c2mir/c2mir.h>

struct JitCode {
    char *code;
    size_t code_size;
    size_t cur_idx; 
};

int get_data(void *data)
{
    struct JitCode *code = (struct JitCode *)data;
    if (code->cur_idx >= code->code_size) {
        return EOF;
    }
    return code->code[code->cur_idx++];
}

const char *test_c = "int add(int a, int b) \
                    {\
                    printf(\"hello world\\n\");\
                    return a + b;\
                    }\n";


typedef struct {
    char *name;
    void (*func)(void);
} func_obj_t;

func_obj_t func_list[] = {
    {"printf", printf},
    {NULL, NULL}
};

void *import_resolver(const char *name)
{
    size_t len = sizeof(func_list) / sizeof(func_obj_t);
    for (int i = 0; len; i++) 
        if (!strcmp(func_list[i].name, name))
            return func_list[i].func;
    return NULL;
}

int binary_search(const char *value)
{
    int l = 0, r = sizeof(func_list) / sizeof(func_obj_t) - 1;
    while(l <= r) {
        int mid = (l + r) / 2;
        int cmp = strcmp(func_list[mid].name, value);
        if (cmp == 0) {
            return mid;
        } else if (cmp < 0) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }
}

typedef  int (*fun_add_t) (int, int);


int main(int argc, char const *argv[])
{
    printf("hello wolrd\n");
    MIR_context_t ctx = MIR_init();
    MIR_gen_init(ctx, 1);
    
    c2mir_init(ctx);
    struct c2mir_options options;
    memset(&options, 0, sizeof(options));
    struct JitCode code;
    code.code = test_c;
    code.code_size = strlen(test_c);
    code.cur_idx = 0;
    if(!c2mir_compile(ctx, &options, get_data, &code, "test.c", NULL))
    {
        printf("compile failed\n");
    }
    c2mir_finish(ctx);

    MIR_module_t module;
    MIR_item_t add_func = NULL;
    for(module = DLIST_HEAD(MIR_module_t, *MIR_get_module_list(ctx)); module != NULL; module = DLIST_NEXT(MIR_module_t, module))
    {
        MIR_item_t func;
        for(func = DLIST_HEAD(MIR_item_t, module->items); func != NULL; func = DLIST_NEXT(MIR_item_t, func))
        {
            if(func->item_type == MIR_func_item && strcmp(func->u.func->name, "add") == 0)
            {
                add_func = func;
            }
            MIR_load_module (ctx, module);
        }
    }
    MIR_gen_set_optimize_level(ctx, 0, 2);
    if(add_func == NULL)
    {
        printf("can not find add function\n");
        return 0;
    }
    fun_add_t fun_add;
    MIR_load_external(ctx, "printf", printf);
    MIR_link (ctx, MIR_set_gen_interface, NULL);
    MIR_gen (ctx, 0, add_func);
    fun_add = add_func->addr;
    int result = fun_add(1, 2);
    printf("result = %d\n", result);
    return 0;
}
