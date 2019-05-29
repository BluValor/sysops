#ifndef ZAD1_FILTER_H
#define ZAD1_FILTER_H


#include "exit_func.h"


typedef struct filter {

    int** v_ints;
    float** v_floats;
    int size;
    int divider;

} filter;


void load_filter(const char* path, filter *buffer);
void save_filter(const char* path, filter *buffer);
void new_filter(filter* buffer, int size);
void delete_filter(filter* buffer);
void prepare_filter(filter *buffer);


#endif //ZAD1_FILTER_H
