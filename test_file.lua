local luamir = require 'luamir'
local test = loadfile('./test file_in.lua')
print(luamir.ljit(test)())