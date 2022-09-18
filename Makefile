CC := gcc
CFLAGS := -Wall -std=gnu11 -g -fPIC -O2
LIBPATH := -L./ -L/usr/local/lib
INCLUDE := -I./mir/

all: test_mir luamir.so luacmatrix.so

test_mir: test_mir.o libmir.a
	$(CC) $(CFLAGS) -o test_mir test_mir.o $(LDFLAGS) $(LIBPATH) -lmir -lpthread

luamir.so: c2luafun.o lua-mir.o libmir.a lua_std_cfunc.o mir_utils.o
	$(CC) $(CFLAGS) -shared -o luamir.so lua-mir.o c2luafun.o lua_std_cfunc.o mir_utils.o $(LDFLAGS) $(LIBPATH) -lmir -lpthread

luacmatrix.so: luacmatrix.o
	$(CC) $(CFLAGS) -shared -o luacmatrix.so luacmatrix.o

luacmatrix.o: luacmatrix.c
	$(CC) $(CFLAGS) -c -o luacmatrix.o luacmatrix.c

c2luafun.o: c2luafun.c c2luafun.h
	$(CC) $(CFLAGS) -c c2luafun.c $(INCLUDE)

lua-mir.o: lua-mir.c c2luafun.h lua_std_cfunc.h
	$(CC) $(CFLAGS) -c lua-mir.c $(INCLUDE)

mir_utils.o: mir_utils.c mir_utils.h
	$(CC) $(CFLAGS) -c mir_utils.c $(INCLUDE)

test_mir.o: test_mir.c
	$(CC) $(CFLAGS) -c test_mir.c $(INCLUDE)

libmir.a:
	cd mir && make -j 4 && cp libmir.a ../ && make clean

lua_std_cfunc.o: lua_std_cfunc.c lua_std_cfunc.h
	$(CC) $(CFLAGS) -c lua_std_cfunc.c $(INCLUDE) -w

lua_std_cfunc.c: export_lua_std_cfunc.lua
	lua export_lua_std_cfunc.lua 
.PHONE: clean
clean:
	-rm -rf *.so.* test_mir *.o *.a *.so && cd mir && make clean


