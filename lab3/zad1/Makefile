CC = gcc

CFLAGS = -c -Wall

DATE = "2019-03-18_23:49"

DIR = "./dir1"

all: main

main: main.o
		$(CC) main.o -o main

main.o: main.c
		$(CC) $(CFLAGS) main.c

run_test:
		make all
		./main $(DIR)

set_up_example:
		rm -rf "dir1"
		mkdir "dir1"
		mkdir "dir1/dir2"
		touch "dir1/dir2/file1.txt"
		touch "dir1/dir2/file2.txt"
		touch "dir1/dir2/file3.txt"
		mkdir "dir1/dir3"
		touch "dir1/dir3/file4.txt"
		touch "dir1/dir3/file5.txt"
		mkdir "dir1/dir4"
		touch "dir1/dir4/file6.txt"
		echo "sample tekst 1 sample tekst 2" > "dir1/dir4/file6.txt"
		mkdir "dir1/dir4/dir5"
		touch "dir1/dir4/dir5/file7.txt"
		touch "dir1/dir4/dir5/file8.txt"
		echo "sample tekst 3" > "dir1/dir4/dir5/file8.txt"