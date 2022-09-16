local file = io.open('/usr/local/include/lua.h', 'r')
for line in file:lines() do
    print(line)
end