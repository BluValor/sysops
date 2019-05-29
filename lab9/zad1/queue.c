#include "queue.h"
#include <stdlib.h>


int create_queue(queue* a_queue, int max) {

    a_queue -> objects = calloc((size_t) max, sizeof(void*));
    a_queue -> head = 0;
    a_queue -> tail = max - 1;
    a_queue -> current = 0;
    a_queue -> max = max;

    return QUEUE_SUCCESS;
}


int destroy_queue(queue* a_queue) {

    free(a_queue -> objects);

    return QUEUE_SUCCESS;
}


int push_queue(queue* a_queue, void* object) {

    if (a_queue -> current == a_queue -> max)
        return QUEUE_FAILURE;

    a_queue -> tail = (a_queue -> tail + 1) % a_queue -> max;
    a_queue -> current++;
    a_queue -> objects[a_queue -> tail] = object;

    return QUEUE_SUCCESS;
}


int pop_queue(queue* a_queue, void** object) {

    if (a_queue -> current == 0)
        return QUEUE_FAILURE;

    *object = a_queue -> objects[a_queue -> head];
    a_queue -> current--;
    a_queue -> head = (a_queue -> head + 1) % a_queue -> max;

    return QUEUE_SUCCESS;
}


int get_first_queue(queue* a_queue, void** object) {


    if (a_queue -> current == 0)
        return QUEUE_FAILURE;

    *object = a_queue -> objects[a_queue -> head];

    return QUEUE_SUCCESS;
}