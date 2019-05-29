CC = gcc
CFLAGS = -Wall
HEADERS = exit_func.h queue.h
SRC = exit_func.c queue.c

.PHONY: clean

all: main

main: $(SRC) $(HEADERS) main.c
	$(CC) $(CFLAGS) $(SRC) main.c -o main -pthread

test: main
	./main 40 3 6 4

clean:
	rm -f main