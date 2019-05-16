//
// Created by bluvalor on 12.03.19.
//

#ifndef UNTITLED1_LIB_FINDER_H
#define UNTITLED1_LIB_FINDER_H

#include <stdio.h>

void allocate_blocks();
int add_block(char* block);
void set_curr_dir(char* dir);
char* get_curr_dir();
void set_curr_doc(char* doc);
char* get_curr_doc();
void set_size(int new_size);
int get_size();
void delete_tmp_file(char* tmp_file_name);
void execute_find(char* tmp_file_name);
size_t get_result_lenght(char* tmp_file_name);
char* get_find_result(char* tmp_file_name);
void free_block(int index);
void clean_up();

#endif //UNTITLED1_LIB_FINDER_H
