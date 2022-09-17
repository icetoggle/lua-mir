local file = io.open('/usr/local/include/lua.h', 'r')
local export_func_list = {}
for line in file:lines() do
    -- %((%w+)%) %(lua_State%* L.*%);
    local func = string.match(line, 'LUA_API[%s%w%*]+%(([a-zA-z_]+)%) %(lua_State %*L.*%);')
    if func then
        table.insert(export_func_list, func)
    end
end

table.sort(export_func_list)
file:close()

local tab_str = '    '
local tabnum = 0
local function writeln(file, str)
    for i = 1, tabnum do
        file:write(tab_str)
    end
    file:write(str)
    file:write('\n')
end

local function inc_tabnum()
    tabnum = tabnum + 1
end

local function dec_tabnum()
    tabnum = tabnum - 1
    assert(tabnum >= 0)
end

local generate_c_file = io.open('lua_std_cfunc.c', 'w')
writeln(generate_c_file, '#include "lua_std_cfunc.h"')
writeln(generate_c_file, '#include "lua.h"')
writeln(generate_c_file, '#include "lauxlib.h"')
writeln(generate_c_file, '#include "lualib.h"')

writeln(generate_c_file, 'typedef struct {')
inc_tabnum()
writeln(generate_c_file, 'const char *name;')
writeln(generate_c_file, 'void (*func)(void);')
dec_tabnum()
writeln(generate_c_file, '} func_obj_t;')

writeln(generate_c_file, 'static func_obj_t func_obj_list[] = {')
inc_tabnum()
for _, func in ipairs(export_func_list) do
    writeln(generate_c_file, '{"' .. func .. '",' .. func .. '},')
end
writeln(generate_c_file, '{NULL, NULL}')
dec_tabnum()
writeln(generate_c_file, '};')


writeln(generate_c_file, 'void *import_luacfun_resolver(const char *name)')
writeln(generate_c_file, '{')
inc_tabnum()
writeln(generate_c_file, 'int l = 0, r = sizeof(func_obj_list) / sizeof(func_obj_t) - 1;')
writeln(generate_c_file, 'while (l <= r) {')
inc_tabnum()
writeln(generate_c_file, 'int m = (l + r) / 2;')
writeln(generate_c_file, 'int cmp = strcmp(name, func_obj_list[m].name);')
writeln(generate_c_file, 'if (cmp == 0) {')
inc_tabnum()
writeln(generate_c_file, 'return func_obj_list[m].func;')
dec_tabnum()
writeln(generate_c_file, '} else if (cmp < 0) {' )
inc_tabnum()
writeln(generate_c_file, 'r = m - 1;')
dec_tabnum()
writeln(generate_c_file, '} else {')
inc_tabnum()
writeln(generate_c_file, 'l = m + 1;')
dec_tabnum()
writeln(generate_c_file, '}')
dec_tabnum()
writeln(generate_c_file, '}')
dec_tabnum()
writeln(generate_c_file, '}')
generate_c_file:close()


