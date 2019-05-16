//
// Created by bluvalor on 12.03.19.
//
#include "block_finder.h"
#include <stdlib.h>

char *CURR_DIR = NULL;
char *CURR_DOC = NULL;

int SIZE = -1;
char** BLOCKS = NULL;

/**
 * allocates BLOCKS array for given SIZE
 */
void allocate_blocks() {

    BLOCKS = calloc(SIZE, sizeof (char*));
}


/**
 * adds given block to the first encountered NULL position in BLOCKS array and returns its index
 **/
int add_block(char* block) {

    for (int i = 0; i < SIZE; i++) {

        if (BLOCKS[i] != NULL)
            continue;

        BLOCKS[i] = block;
        return i;
    }

    return -1;
}


/**
 * sets CURR_DIR to given char[]
 */
void set_curr_dir(char* dir) {

    CURR_DIR = dir;
}


char* get_curr_dir() {
    return CURR_DIR;
}


/**
 * sets CURR_DOC to given char[]
 */
void set_curr_doc(char* doc) {

    CURR_DOC = doc;
}


char* get_curr_doc() {
    return CURR_DOC;
}


/**
 * sets SIZE to given int
 */
void set_size(int new_size) {

    SIZE = new_size;
}


int get_size() {

    return SIZE;
}


/**
 * deletes temporary file with given name
 */
void delete_tmp_file(char* tmp_file_name) {

    char delete_str[64];
    snprintf(delete_str, sizeof delete_str, "%s%s", "rm -f ", tmp_file_name);

    system(delete_str);
}


/**
 * executes find with given global arguments and saves result to the temporary file with the given name
 */
void execute_find(char* tmp_file_name) {

    delete_tmp_file(tmp_file_name);

    char find_str[256];
    snprintf(find_str, sizeof find_str, "%s%s%s%s%s%s%s", "find ", CURR_DIR, " -name '", CURR_DOC, "' 1>> ", tmp_file_name, " 2>> /dev/null");

    system(find_str);
}


/**
 * returns lenght of text in given temporary file name
 */
size_t get_result_lenght(char* tmp_file_name) {

    FILE *doc = fopen(tmp_file_name, "r");

    size_t result = 0;;

    int single_char;

    single_char = getc(doc);

    while (single_char != EOF) {

        single_char = getc(doc);
        result++;
    }

    fclose(doc);

    return result;
}


/**
 * returns char[] value of the find result from the temporary file with the given name
 */
char* get_find_result(char* tmp_file_name) {

    char *result = (char*) calloc(get_result_lenght(tmp_file_name), sizeof (char));

    size_t index = 0;

    FILE *doc = fopen(tmp_file_name, "r");

    char single_char;
    single_char = (char) getc(doc);

    while (single_char != EOF) {

        result[index++] = single_char;
        single_char = (char) getc(doc);
    }

    fclose(doc);

    return result;
}


/**
 *  frees BLOCKS[index] and assigns it to NULL
 */
void free_block(int index) {

    free(BLOCKS[index]);
    BLOCKS[index] = NULL;
}


/**
 * frees all blocks and deletes temporary file
 */
void clean_up() {

    for (int i = SIZE - 1; i >= 0; i--) {
        if (BLOCKS[i] != NULL)
            free(BLOCKS[i]);
    }

    free(BLOCKS);
}