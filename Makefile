CC := gcc
CFLAGS := -Wall -std=gnu11 -g -fPIC -O2 -DLUA_LIB
LIBPATH := -L./
INCLUDE := -I./mir/ -I./lua-src/

all: test_mir luamir.so luacmatrix.so

test_mir: test_mir.o libmir.a
	$(CC) $(CFLAGS) -o test_mir test_mir.o $(LDFLAGS) $(LIBPATH) -lmir -lpthread

luamir.so: c2cluafunc.o lua-mir.o libmir.a liblua.a lua_std_cfunc.o mir_utils.o lua2cluafunc.o membuf.o parse_opcode.o testfunc.o luavm_utils.o luamir_ctx.o array.o
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) $(LIBPATH) -lmir -lpthread -llua

luacmatrix.so: luacmatrix.o
	$(CC) $(CFLAGS) -shared -o luacmatrix.so luacmatrix.o

luacmatrix.o: luacmatrix.c
	$(CC) $(CFLAGS) -c -o luacmatrix.o luacmatrix.c

c2cluafunc.o: c2cluafunc.c c2cluafunc.h
	$(CC) $(CFLAGS) -c c2cluafunc.c $(INCLUDE)

lua-mir.o: lua-mir.c c2cluafunc.h lua_std_cfunc.h lua2cluafunc.h
	$(CC) $(CFLAGS) -c lua-mir.c $(INCLUDE)

mir_utils.o: mir_utils.c mir_utils.h
	$(CC) $(CFLAGS) -c mir_utils.c $(INCLUDE)

test_mir.o: test_mir.c
	$(CC) $(CFLAGS) -c test_mir.c $(INCLUDE)

libmir.a:
	cd mir && make -j 4 && cp libmir.a ../ && make clean

liblua.a:
	cd lua-src && make -j 4 && cp liblua.a ../ && cp lua ../lua && make clean

lua_std_cfunc.o: lua_std_cfunc.c lua_std_cfunc.h
	$(CC) $(CFLAGS) -c lua_std_cfunc.c $(INCLUDE) -w

lua_std_cfunc.c: export_lua_std_cfunc.lua
	lua export_lua_std_cfunc.lua 

lua2cluafunc.o: lua2cluafunc.c lua2cluafunc.h
	$(CC) $(CFLAGS) -c lua2cluafunc.c $(INCLUDE)

membuf.o: membuf.c membuf.h
	$(CC) $(CFLAGS) -c membuf.c $(INCLUDE)

parse_opcode.o: parse_opcode.c
	$(CC) $(CFLAGS) -c parse_opcode.c $(INCLUDE)

testfunc.o: testfunc.c
	$(CC) $(CFLAGS) -c testfunc.c $(INCLUDE)

luavm_utils.o: luavm_utils.c luavm_utils.h
	$(CC) $(CFLAGS) -c luavm_utils.c $(INCLUDE)


luamir_ctx.o: luamir_ctx.c luamir_ctx.h
	$(CC) $(CFLAGS) -c luamir_ctx.c $(INCLUDE)

array.o: array.c array.h
	$(CC) $(CFLAGS) -c array.c $(INCLUDE)

.PHONE: clean
clean:
	-rm -rf *.so.* test_mir *.o *.a *.so lua && cd mir && make clean


