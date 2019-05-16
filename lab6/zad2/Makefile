CC = gcc

CFLAGS = -Wall -c

all: posix_messages.o posix_project_functions.o server client

server: posix_server.o
		$(CC) posix_server.o -o server -lrt

posix_server.o: posix_server.c
		$(CC) $(CFLAGS) posix_server.c

client: posix_client.o
		$(CC) posix_client.o -o client -lrt

posix_client.o: posix_client.c
		$(CC) $(CFLAGS) posix_client.c

posix_project_functions.o: posix_project_functions.h
		$(CC) $(CFLAGS) posix_project_functions.h

posix_messages.o: posix_messages.h
		$(CC) $(CFLAGS) posix_messages.h

clean:
		rm -f *.o