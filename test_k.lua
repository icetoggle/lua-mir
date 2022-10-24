local function test_pushclosure(a, b)
    local function test(a, b)
        return a + b
    end
    return test(a, b)
end
local luamir = require('luamir')
print(luamir.lua2c(test_pushclosure))
print(luamir.ljit(test_pushclosure)(1,2))
