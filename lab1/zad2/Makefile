CC=gcc

CFLAGS=-c -Wall

all: main

main: main.o lib_finder.o
		$(CC) main.o lib_finder.o -o main

main.o: main.c
		$(CC) $(CFLAGS) main.c

lib_finder.o: lib_finder.c
		$(CC) $(CFLAGS) lib_finder.c

