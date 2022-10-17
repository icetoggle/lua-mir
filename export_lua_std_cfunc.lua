local export_func_list = {}

local black_func = {
    ["luaH_mainposition"] = true,
    ["luaH_isdummy"] = true,
}

local function gen_import_func(path)
    local file = io.open(path, 'r')
    for line in file:lines() do
        -- %((%w+)%) %(lua_State%* L.*%);
        local func = string.match(line, 'LUALIB_API[%w_%s*]+%(([a-zA-z_]+)%) %(.*')
        if not func then
            func = string.match(line, 'LUAI_FUNC [%w_%s*]+ %*?([a-zA-z_]+) %(.*')
        end
        if not func then
            func = string.match(line, 'LUA_API [%w_]+ %(([a-zA-z_]+)%) %(.*')
        end
        if black_func[func] then
            goto continue
        end
        if func then
            print(func, line)
            table.insert(export_func_list, func)
        end
        ::continue::
    end
    file:close()
end
local import_func_h = {
    "lauxlib.h",
    "ldebug.h",
    "ldo.h",
    "lfunc.h",
    "lgc.h",
    "lmem.h",
    "lobject.h",
    "lstate.h",
    "lstring.h",
    "ltable.h",
    "ltm.h",
    "lua.h",
    "lundump.h",
    "lvm.h",
    "ltm.h",
}
for _, h in ipairs(import_func_h) do
    gen_import_func("lua-src/" .. h)
end

table.insert(export_func_list, 'malloc')
table.insert(export_func_list, 'free')
table.insert(export_func_list, 'printf')
table.insert(export_func_list, 'memcpy')
table.insert(export_func_list, 'fmod')
table.insert(export_func_list, 'pow')
table.insert(export_func_list, 'floor')
table.insert(export_func_list, 'l_strcmp')
table.insert(export_func_list, 'lessthanothers')
table.insert(export_func_list, 'lessequalothers')
table.insert(export_func_list, 'forprep')
table.insert(export_func_list, 'floatforloop')
table.insert(export_func_list, 'pushclosure')

table.sort(export_func_list)

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
for _, h in ipairs(import_func_h) do
    writeln(generate_c_file, '#include "' .. h .. '"')
end

writeln(generate_c_file, '#include "string.h"')
writeln(generate_c_file, '#include "stdlib.h"')
writeln(generate_c_file, '#include "math.h"')
writeln(generate_c_file, '#include "luavm_utils.h"')
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
writeln(generate_c_file, "printf(\"import_luacfun_resolver: %s not found\\n\", name);");
dec_tabnum()
writeln(generate_c_file, '}')

writeln(generate_c_file, 'const char* lua_header_list[] = {')
inc_tabnum();
for _, h in ipairs(import_func_h) do
    writeln(generate_c_file, '"'.. h .. '",')
end
dec_tabnum()
writeln(generate_c_file, '};')
generate_c_file:close()


