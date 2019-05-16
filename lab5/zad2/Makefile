CC = gcc

CFLAGS = -c -Wall

all: master slave

master: master.o
		$(CC) master.o -o master

master.o: master.c
		$(CC) $(CFLAGS) master.c

slave: slave.o
		$(CC) slave.o -o slave

slave.o: slave.c
		$(CC) $(CFLAGS) slave.c