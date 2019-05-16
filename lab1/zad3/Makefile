CC=gcc

CFLAGS=-c -Wall

static: main

main: main.o block_finder.o
		$(CC) main.o block_finder.o -o main

main.o: main.c
		$(CC) $(CFLAGS) main.c

block_finder.o: block_finder.c
		$(CC) $(CFLAGS) block_finder.c

shared: block_finder.c
		$(CC) $(CFLAGS) -fPIC block_finder.c
		$(CC) $(CFLAGS) -shared -fPIC block_finder.c -o block_finder.so

run_static: 
		make static
		$(CC) -o out_static main.c block_finder.o
		make test_static

test_static:
		LD_LIBRARY_PATH=./ ./out_static create_table 500 search_directory /etc doc* tmp_male_zglebienia search_directory /usr doc* tmp_srednie_zglebienia search_directory / doc* tmp_duze_zglebienia create_block tmp_male_zglebienia insert_block create_block tmp_srednie_zglebienia insert_block create_block tmp_duze_zglebienia insert_block remove_block 0 remove_block 1 remove_block 2 add_and_delete_n_blocks 100 tmp_male_zglebienia add_and_delete_n_blocks 2000 tmp_male_zglebienia add_and_delete_n_blocks 5000 tmp_male_zglebienia add_and_delete_n_blocks 100 tmp_srednie_zglebienia add_and_delete_n_blocks 200 tmp_srednie_zglebienia add_and_delete_n_blocks 500 tmp_srednie_zglebienia add_and_delete_n_blocks 100 tmp_duze_zglebienia add_and_delete_n_blocks 200 tmp_duze_zglebienia add_and_delete_n_blocks 500 tmp_duze_zglebienia

clean:
		rm -f *.o
		rm -f *.a
		rm -f *.gch
		rm -f *.so
		rm -f *.txt
		rm -f .so.
		rm -f tmp*
