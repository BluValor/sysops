CC = gcc

CFLAGS = -c -Wall

all: main

main: main.o
		$(CC) main.o -o main

main.o: main.c
		$(CC) $(CFLAGS) main.c

run_test:
		make all
		./main ./test_file.txt 1 4 16

