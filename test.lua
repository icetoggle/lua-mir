local function add(a, b)
    return a + b
end

local luamir = require 'luamir'
print(luamir.getlclosure(add)(1, 2))
print(luamir.lua2c(add));