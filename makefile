libmem.so: mem.h mem.c
	gcc -c -fpic mem.c -Wall -Werror
	gcc -shared -o libmem.so mem.o

memtest: memtest.c
	gcc -L. -o memtest memtest.c -Wall -lmem

all: libmem.so memtest
