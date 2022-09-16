CC := gcc
CFLAGS := -Wall -std=gnu11 -g -fPIC
LIBPATH := -L./
INCLUDE := -I./mir/

test_mir: mir test_mir.o libmir.a
	$(CC) $(CFLAGS) -o test_mir test_mir.o $(LDFLAGS) $(LIBPATH) -lmir -lpthread

test_mir.o: test_mir.c
	$(CC) $(CFLAGS) -c test_mir.c $(INCLUDE)

libmir.a: mir
	cd mir && make -j 4 && cp libmir.a ../ && make clean

.PHONE: clean
clean:
	-rm -rf *.so.* test_mir *.o *.a *.so && cd mir && make clean


