CC = gcc

CFLAGS = -c -Wall

all: main

main: main.o
		$(CC) main.o -o main

main.o: main.c
		$(CC) $(CFLAGS) main.c

run_test:
		make all
		./main generate tmp_800_1 800 1 generate tmp_4500_1 4500_1 1 report "Record size - 1" copy tmp_800_1 tmp_800_1_copy 800 1 sys copy tmp_800_1 tmp_800_1_copy 800 1 lib copy tmp_4500_1 tmp_4500_1_copy 4500 1 sys copy tmp_4500_1 tmp_4500_1_copy 4500 1 lib sort tmp_800_1 800 1 sys sort tmp_800_1_copy 800 1 lib sort tmp_4500_1 4500 1 sys sort tmp_4500_1_copy 4500 1 lib generate tmp_800_4 800 4 generate tmp_4500_4 4500_4 4 report "Record size - 4" copy tmp_800_4 tmp_800_4_copy 800 4 sys copy tmp_800_4 tmp_800_4_copy 800 4 lib copy tmp_4500_4 tmp_4500_4_copy 4500 4 sys copy tmp_4500_4 tmp_4500_4_copy 4500 4 lib sort tmp_800_4 800 4 sys sort tmp_800_4_copy 800 4 lib sort tmp_4500_4 4500 4 sys sort tmp_4500_4_copy 4500 4 lib generate tmp_800_512 800 512 generate tmp_4500_512 4500_512 512 report "Record size - 512" copy tmp_800_512 tmp_800_512_copy 800 512 sys copy tmp_800_512 tmp_800_512_copy 800 512 lib copy tmp_4500_512 tmp_4500_512_copy 4500 512 sys copy tmp_4500_512 tmp_4500_512_copy 4500 512 lib sort tmp_800_512 800 512 sys sort tmp_800_512_copy 800 512 lib sort tmp_4500_512 4500 512 sys sort tmp_4500_512_copy 4500 512 lib generate tmp_800_1024 800 1024 generate tmp_4500_1024 4500_1024 1024 report "Record size - 1024" copy tmp_800_1024 tmp_800_1024_copy 800 1024 sys copy tmp_800_1024 tmp_800_1024_copy 800 1024 lib copy tmp_4500_1024 tmp_4500_1024_copy 4500 1024 sys copy tmp_4500_1024 tmp_4500_1024_copy 4500 1024 lib sort tmp_800_1024 800 1024 sys sort tmp_800_1024_copy 800 1024 lib sort tmp_4500_1024 4500 1024 sys sort tmp_4500_1024_copy 4500 1024 lib generate tmp_800_4096 800 4096 generate tmp_4500_4096 4500_4096 4096 report "Record size - 4096" copy tmp_800_4096 tmp_800_4096_copy 800 4096 sys copy tmp_800_4096 tmp_800_4096_copy 800 4096 lib copy tmp_4500_4096 tmp_4500_4096_copy 4500 4096 sys copy tmp_4500_4096 tmp_4500_4096_copy 4500 4096 lib sort tmp_800_4096 800 4096 sys sort tmp_800_4096_copy 800 4096 lib sort tmp_4500_4096 4500 4096 sys sort tmp_4500_4096_copy 4500 4096 lib generate tmp_800_8192 800 8192 generate tmp_4500_8192 4500_8192 8192 report "Record size - 8192" copy tmp_800_8192 tmp_800_8192_copy 800 8192 sys copy tmp_800_8192 tmp_800_8192_copy 800 8192 lib copy tmp_4500_8192 tmp_4500_8192_copy 4500 8192 sys copy tmp_4500_8192 tmp_4500_8192_copy 4500 8192 lib sort tmp_800_8192 800 8192 sys sort tmp_800_8192_copy 800 8192 lib sort tmp_4500_8192 4500 8192 sys sort tmp_4500_8192_copy 4500 8192 lib

