local function add(a, b)
    return a + b
end

local luamir = require 'luamir'
--print(luamir.lua2c(add))
print(luamir.ljit(add)(1,2));
add(1, 2)