CC = gcc

CFLAGS = -Wall -c

all: systemv_messages.o systemv_project_functions.o server client

server: systemv_server.o
		$(CC) systemv_server.o -o server -lrt

systemv_server.o: systemv_server.c
		$(CC) $(CFLAGS) systemv_server.c

client: systemv_client.o
		$(CC) systemv_client.o -o client -lrt

systemv_client.o: systemv_client.c
		$(CC) $(CFLAGS) systemv_client.c

systemv_project_functions.o: systemv_project_functions.h
		$(CC) $(CFLAGS) systemv_project_functions.h

systemv_messages.o: systemv_messages.h
		$(CC) $(CFLAGS) systemv_messages.h

clean:
		rm -f *.o