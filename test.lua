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

function test_addk(a)
    local c = a + 1000000000
    return c
end

local function test_mulk(a)
    return a * 1000000000
end
local function test_subk(a)
    return a - 1000000000
end
local function test_divk(a)
    return a / 1000000000
end
local function test_divvk(a)
    return a // 1000000000
end
local function test_modk(a)
    return a % 1000000000
end
local function test_powk(a)
    return a ^ 8
end
local function test_bank(a)
    return a & 3
end
local function test_bork(a)
    return a | 2
end
local function test_bxork(a)
    return a ~ 3
end

local function test_shl(a)
    return a >> 1
end

local function test_shr(a)
    return a << 1
end

local function test_band(a, b)
    return a & b
end

local function test_bor(a, b)
    return a | b
end

local function test_bxor(a, b)
    return a ~ b
end

local function test_shl(a, b)
    return a >> b
end

local function test_shr(a, b)
    return a << b
end

local function test_unm(a)
    local c = -a
    return c
end

local function test_not(a)
    local c = not a
    return c
end

local function test_len(a)
    local c = #a
    return c
end

local function test_concat(a, b)
    local c = a .. b
    return c
end

local function test_eq(a, b)
    local c = a == b
    return c
end
local luamir = require 'luamir'
print("add", luamir.ljit(add)(1,2) == 3);
print("sub", luamir.ljit(sub)(1,2) == -1);
print("mul", luamir.ljit(mul)(1,2) == 2);
print("mod", luamir.ljit(mod)(1,2) == 1);
print("div", luamir.ljit(div)(1,2) == 0.5);
print("divv", luamir.ljit(divv)(1,2) == 0);
print("pow", luamir.ljit(pow)(1,10000) == 1);
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
print("test_addk", luamir.ljit(test_addk)(1) == 1000000001);
print("test_addk", luamir.ljit(test_addk)(a).x == 1000000001 and luamir.ljit(test_addk)(a).y == 1000000002);

print("test_subk", luamir.ljit(test_subk)(1) == -999999999);
print("test_mulk", luamir.ljit(test_mulk)(1) == 1000000000);
print("test_divk", luamir.ljit(test_divk)(1) == 1/1000000000);
print("test_divvk", luamir.ljit(test_divvk)(1) == 0);
print("test_modk", luamir.ljit(test_modk)(1) == 1);

print("test_powk", luamir.ljit(test_powk)(2) == 256);
print("test_bank", luamir.ljit(test_bank)(1) == 1);
print("test_bork", luamir.ljit(test_bork)(1) == 3);
print("test_bxork", luamir.ljit(test_bxork)(1) == 2);

print("test_band", luamir.ljit(test_band)(1, 2) == 0);
print("test_bor", luamir.ljit(test_bor)(1, 2) == 3);
print("test_bxor", luamir.ljit(test_bxor)(1, 2) == 3);

-- print("test_shl", luamir.lua2c(test_shl));
print("test_shl", luamir.ljit(test_shl)(1, 2) == 0);
print("test_shr", luamir.ljit(test_shr)(1, 2) == 4);
print("test_unm", luamir.ljit(test_unm)(1) == -1);
print("test_not", luamir.ljit(test_not)(false) == true);
print("test_len", luamir.ljit(test_len)({1,2,3}) == 3);

print("test_concat", luamir.ljit(test_concat)("1", "2") == "12");
print("test_eq", luamir.ljit(test_eq)(1, 1) == true);