#ifndef SYSOPY_LAB_9_QUEUE_H
#define SYSOPY_LAB_9_QUEUE_H


#define QUEUE_SUCCESS 0
#define QUEUE_FAILURE -1


typedef struct queue {

    void** objects;
    int head, tail, current, max;

} queue;


int create_queue(queue* a_queue, int max);
int destroy_queue(queue* a_queue);
int push_queue(queue* a_queue, void* object);
int pop_queue(queue* a_queue, void** object);
int get_first_queue(queue* a_queue, void** object);


#endif //SYSOPY_LAB_9_QUEUE_H
