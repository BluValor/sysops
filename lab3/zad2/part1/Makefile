CC = gcc

CFLAGS = -c -Wall

ARG1 = "./list.txt"

ARG2 = "10"

all: main

main: main.o
		$(CC) main.o -o main

main.o: main.c
		$(CC) $(CFLAGS) main.c

run_test_memory:
		make all
		./main $(ARG1) $(ARG2) memory_cpy

run_test_exec:
		make all
		./main $(ARG1) $(ARG2) exec_cpy
