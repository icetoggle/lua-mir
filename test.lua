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

local function test_gettable(t)
    local a = 'a' 
    local c = t[a]
    return c
end

local function test_SETTABLEUP()
    t[1] = 2
    return t[1]
end

local function test_settable()
    local c = 1
    t[c] = 2
    return t[c]
end

local function test_setfield(t)
    t.a  = 3
    return t.a
end

local function test_newtable()
    local t = {}
    t[1] = 12333
    return t[1]
end

function t:test_self()
    local r = self:test_self2()
    return r
end

function t:test_self2()
    return 2
end

function test_addi(a)
    local c = a + 1
    return c
end
local luamir = require 'luamir'
print("add", luamir.ljit(add)(1,2) == 3);
print("sub", luamir.ljit(sub)(1,2) == -1);
print("mul", luamir.ljit(mul)(1,2) == 2);
print("mod", luamir.ljit(mod)(1,2) == 1);
print("div", luamir.ljit(div)(1,2) == 0.5);
print("divv", luamir.ljit(divv)(1,2) == 0);
print("pow", luamir.ljit(pow)(1,2) == 1.0);
print("test1", luamir.ljit(test1)(1,2) == 4);
print("test2", luamir.ljit(test2)(1,2) == "111111111111");
print("test_false", luamir.ljit(test_false)(1,2) == true);
print("testnil", luamir.ljit(test_nil)() == 3);
print("test_up", luamir.ljit(test_up)() == "test");
print("test_setup", luamir.ljit(test_setup)() == "test2");
print("test_tup", luamir.ljit(test_tup)() == 1);
print("test_geti", luamir.ljit(test_geti)(t) == 2);
print("test_getfield", luamir.ljit(test_getfield)(t) == 1);
print("test_gettable", luamir.ljit(test_gettable)(t) == 1);
print("test_SETTABLEUP", luamir.ljit(test_SETTABLEUP)() == 2);
print("test_settable", luamir.ljit(test_settable)() == 2);
print("test_setfield", luamir.ljit(test_setfield)(t) == 3);
print("test_newtable", luamir.ljit(test_newtable)(t) == 12333);
print("test_self", luamir.ljit(t.test_self)(t) == 2);
print("test_addi", luamir.ljit(test_addi)(1) == 2);
local a = {x = 1, y = 2}
setmetatable(a, {__add = function(t, a) return {x = t.x + a, y = t.y + a} end})
print("test_addi_metatable", luamir.ljit(test_addi)(a).x == 2 and luamir.ljit(test_addi)(a).y == 3);
