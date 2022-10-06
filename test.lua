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

local function test_false()
    local c = true
    return c
end

local s = "test"

local function test_nil()
    local a = 1
    local b = 2
    local c = 3
    local d = 4
    return c
end

local function test_up(a, b)
    local c = s
    return c
end

local function test_setup(a, b)
    s = "test2"
    local c = s
    return c
end


local t = {[1] = 2, a = 1}
local function test_tup()
    local c = t.a 
    return c
end

local function test_geti(t) 
    local c = t[1]
    return c
end

local function test_getfield(t) 
    local c = t.a
    return c
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
print("test2", luamir.ljit(test2)(1,2));
print("test_false", luamir.ljit(test_false)(1,2));
print("testnil", luamir.ljit(test_nil)());
print("test_up", luamir.ljit(test_up)());
print("test_setup", luamir.ljit(test_setup)());
print("test_tup", luamir.ljit(test_tup)());
print("test_geti", luamir.ljit(test_geti)(t));
print("test_getfield", luamir.ljit(test_getfield)(t));
