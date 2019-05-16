#ifndef ZAD1_CBELT_H
#define ZAD1_CBELT_H


#include <sys/ipc.h>
#include "package.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_CAPACITY 512
#define CBELT_SIZE sizeof(cbelt)


typedef struct cbelt {

    int load, capacity, weight, max_weight, head, tail;
    key_t sem_key;
    package spots[MAX_CAPACITY];

} cbelt;


cbelt make_cbelt(int capacity, int max_weight, key_t sem_key);
int cbelt_put(cbelt* a_cbelt, long sem_id, package a_package);
package* cbelt_take(cbelt *a_cbelt, long sem_id, package* a_package);


#endif //ZAD1_CBELT_H
