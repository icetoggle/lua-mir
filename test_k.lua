local function test_setlist(n)
    local a = n
    local t = {1, 2, 3, 4}
    return t[2]
end
local luamir = require('luamir')
print(luamir.lua2c(test_setlist))
print(luamir.ljit(test_setlist)(3))
