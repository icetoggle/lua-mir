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

local luamir = require 'luamir'
--print(luamir.lua2c(add))
print("add", luamir.ljit(add)(1,2));
print("sub", luamir.ljit(sub)(1,2));
print("mul", luamir.ljit(mul)(1,2));
print("mod", luamir.ljit(mod)(1,2));
print("div", luamir.ljit(div)(1,2));
print("divv", luamir.ljit(divv)(1,2));
print("pow", luamir.ljit(pow)(1,2));
add(1, 2)