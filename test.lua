local function add(a, b)
    return a + b
end
local function mul(a, b)
    return a * b
end
local function sub(a, b)
    return a - b
end
local function div(a, b)
    return a / b
end
local function divv(a, b)
    return a // b
end
local function mod(a, b)
    return a % b
end
local function pow(a, b)
    return a ^ b
end
local function add(a, b)
    return a + b
end

local function test1(a, b)
    local c = a + b
    local d = 1
    c = c + d
    return c
end

local function test2(a, b)
    local c = a + b
    local d = "111111111111"
    return d
end

local luamir = require 'luamir'
print("add", luamir.ljit(add)(1,2));
print("sub", luamir.ljit(sub)(1,2));
print("mul", luamir.ljit(mul)(1,2));
print("mod", luamir.ljit(mod)(1,2));
print("div", luamir.ljit(div)(1,2));
print("divv", luamir.ljit(divv)(1,2));
print("pow", luamir.ljit(pow)(1,2));
print("test1", luamir.ljit(test1)(1,2));
print(luamir.lua2c(test2))
print("test2", luamir.ljit(test2)(1,2));
add(1, 2)