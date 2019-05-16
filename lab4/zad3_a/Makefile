CC = gcc

CFLAGS = -Wall -c

all: shared_func.o catcher sender

catcher: catcher.o
		$(CC) catcher.o -o catcher

catcher.o: catcher.c
		$(CC) $(CFLAGS) catcher.c

sender: sender.o
		$(CC) sender.o -o sender

sender.o: sender.c
		$(CC) $(CFLAGS) sender.c

shared_func.o: shared_func.c
		$(CC) $(CFLAGS) shared_func.c

clean:
		rm -f *.o