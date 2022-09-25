local luamir = require 'luamir'
local u = {x = 1, y = 1}
local v = {x = 2, y = 2}
local mt = {__add = function(l, r) 
    return {x = l.x + r.x, y = l.y + r.y}
end}

setmetatable(u, mt)
setmetatable(v, mt)

local function add(a, b)
    local c = a + b
    return c
end
local ret = luamir.ljit(add)(u, v)
print(ret.x, ret.y)
