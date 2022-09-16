CC := gcc
CFLAGS := -Wall -std=gnu11 -g -fPIC
LIBPATH := -L./ -L/usr/local/lib
INCLUDE := -I./mir/

all: test_mir luamir.so

test_mir: test_mir.o libmir.a
	$(CC) $(CFLAGS) -o test_mir test_mir.o $(LDFLAGS) $(LIBPATH) -lmir -lpthread

luamir.so: c2luafun.o lua-mir.o libmir.a
	$(CC) $(CFLAGS) -shared -o luamir.so lua-mir.o c2luafun.o $(LDFLAGS) $(LIBPATH) -lmir -lpthread

c2luafun.o: c2luafun.c c2luafun.h
	$(CC) $(CFLAGS) -c c2luafun.c $(INCLUDE)

lua-mir.o: lua-mir.c c2luafun.h
	$(CC) $(CFLAGS) -c lua-mir.c $(INCLUDE)


test_mir.o: test_mir.c
	$(CC) $(CFLAGS) -c test_mir.c $(INCLUDE)

libmir.a:
	cd mir && make -j 4 && cp libmir.a ../ && make clean

.PHONE: clean
clean:
	-rm -rf *.so.* test_mir *.o *.a *.so && cd mir && make clean


