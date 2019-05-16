CC=gcc

CFLAGS=-c -Wall

all: lib_finder

lib_finder.a: lib_finder.c
		$(CC) $(CFLAGS) lib_finder.c
		ar rcs lib_finder.a lib_finder.o

static_lib: libfinder.a
