local n = 100000000
local function fab(n)
   local a = 1
   local b = 1
   for i = 1, n do
      a, b = b, (a + b) % 100
   end 
   return b
end
local t1 = os.clock()
print(fab(n))
local t2 = os.clock()
print("time: ", t2 - t1)

local luamir = require 'luamir'
-- print(luamir.lua2c(fab))
local jitfab = luamir.ljit(fab)
local t1 = os.clock()
print(jitfab(n))
local t2 = os.clock()
print("time: ", t2 - t1)