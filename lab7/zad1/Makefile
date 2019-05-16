CC      := gcc
CFLAGS  := -Wall
HFILES  := exit_func.h shared_func.h ipc_func.h cbelt.h package.h
CFILES  := exit_func.c shared_func.c ipc_systemv.c cbelt.c package.c

.PHONY: all clean
.DEFAULT: all

all: truck worker boss

truck: $(HFILES) $(CFILES) truck.c
	$(CC) $(CFLAGS) $(CFILES) truck.c -o truck

worker: $(HFILES) $(CFILES) worker.c
	$(CC) $(CFLAGS) $(CFILES) worker.c -o worker

boss: $(HFILES) $(CFILES) boss.c
	$(CC) $(CFLAGS) $(CFILES) boss.c -o boss

clean:
	$(RM) truck worker boss
