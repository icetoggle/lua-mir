local function add(a, b)
    return a + b
end

local luamir = require 'luamir'
print(luamir.ljit(add)(1,2));