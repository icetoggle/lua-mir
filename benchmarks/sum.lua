local function sum(n)
    local sum = 0
    for i = 1, n do
        sum = (sum + i) * i
    end
end
    

local t1 = os.clock()
sum(100000000)
local t2 = os.clock()
print("time: ", t2 - t1)

local luamir = require 'luamir'
local jitsum = luamir.ljit(sum)
local t1 = os.clock()
jitsum(100000000)
local t2 = os.clock()
print("time: ", t2 - t1)