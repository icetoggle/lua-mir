local jit = require 'luamir'
local test = jit.cjit('test', [[
    #include "lua.h"
    int test(lua_State *L) {
        lua_pushstring(L, "Hello, World!");
        return 1;
    }
]]);
print(test())