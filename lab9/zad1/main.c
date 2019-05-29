#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include <unistd.h>
#include "exit_func.h"
#include "queue.h"


time_t start_time;
int trolleys_left, max_trolley_load, rides_count;
queue *passengers_queue, *trolleys_queue;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_waiting_for_passengers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_waiting_for_passenger_to_enter = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passenger_entering = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_waiting_for_start = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passengers_preparing_to_start = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passengers_waiting_to_leave = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passenger_left = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_new_trolley_can_approach = PTHREAD_COND_INITIALIZER;


enum action{
    INITIAL, READY_OUT, BOARDING, IDLE_IN, IDLE_START, MOVING, MOVING_STARTED, READY_IN
};


typedef struct person person;


typedef struct trolley {

    int id, load;
    person** passengers;
    pthread_t thread;

} trolley;


struct person {

    int id, action;
    trolley* a_trolley;
    pthread_t thread;

};


int read_int_value(const char* input, int *buffer) {

    char *end = NULL;
    errno = 0;
    long result = strtol(input, &end, 10);
    if (result >= INT_MAX || end == input || errno != 0)
        return -1;
    *buffer = (int) result;
    return 0;
}

time_t get_current_time() {

    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    return current_time . tv_sec * 1000 + current_time . tv_usec / 1000;
}


void print_message(const char *template, ...) {

    va_list args;
    va_start(args, template);
    char buffer[256];
    snprintf(buffer, 256, "%s Timestamp: %ld\n", template, get_current_time() - start_time);
    vfprintf(stderr, buffer, args);
    va_end(args);
}


void* passenger_loop(void* data) {

    person* a_person = (person*) data;

    pthread_mutex_lock(&mutex);

    while (1) {

        while (a_person -> action != READY_OUT && trolleys_left > 0)
            pthread_cond_wait(&cond_waiting_for_passenger_to_enter, &mutex);

        if (trolleys_left == 0)
            break;

        print_message("Passenger nr. %d is entering trolley. Current passengers amount: %d.",
                      a_person -> id, a_person -> a_trolley -> load + 1);

        a_person -> action = BOARDING;
        pthread_cond_broadcast(&cond_passenger_entering);

        while (a_person -> action != IDLE_IN && a_person -> action != IDLE_START)
            pthread_cond_wait(&cond_waiting_for_start, &mutex);

        if (a_person -> action == IDLE_START){

            print_message("Passenger nr. %d is pushing start button.", a_person -> id);
            a_person -> action = MOVING_STARTED;

        } else if (a_person -> action == IDLE_IN)
            a_person -> action = MOVING;

        pthread_cond_broadcast(&cond_passengers_preparing_to_start);

        while (a_person -> action != READY_IN)
            pthread_cond_wait(&cond_passengers_waiting_to_leave, &mutex);

        print_message("Passenger nr. %d is getting out of the trolley. Current passengers amount: %d.",
                      a_person -> id, a_person -> a_trolley -> load - 1);

        a_person -> action = INITIAL;
        pthread_cond_broadcast(&cond_passenger_left);
        push_queue(passengers_queue, a_person);
    }

    print_message("Passenger nr. %d is closing it's thread.", a_person -> id);

    pthread_mutex_unlock(&mutex);
    return 0;
}


void* trolley_loop(void* data) {

    trolley* a_trolley = (trolley*) data;

    pthread_mutex_lock(&mutex);

    trolley* object;
    while (get_first_queue(trolleys_queue, (void**) &object) == QUEUE_SUCCESS && object != a_trolley)
        pthread_cond_wait(&cond_new_trolley_can_approach, &mutex);

    for (int i = 0; i < rides_count; i++) {

        print_message("Trolley nr. %d is opening doors.", a_trolley -> id);

        while (a_trolley -> load != max_trolley_load) {

            person* a_person;

            while (pop_queue(passengers_queue, (void**) &a_person) == QUEUE_FAILURE)
                pthread_cond_wait(&cond_waiting_for_passengers, &mutex);

            a_person -> action = READY_OUT;
            a_person -> a_trolley = a_trolley;

            pthread_cond_broadcast(&cond_waiting_for_passenger_to_enter);

            while (a_person -> action != BOARDING)
                pthread_cond_wait(&cond_passenger_entering, &mutex);

            a_trolley -> passengers[a_trolley -> load++] = a_person;
            a_person -> action = IDLE_IN;
        }

        print_message("Trolley nr. %d is closing doors.", a_trolley -> id);

        int passenger_to_push_start = rand() % max_trolley_load;
        a_trolley -> passengers[passenger_to_push_start] -> action = IDLE_START;

        for (int j = 0; j < max_trolley_load; j++) {

            pthread_cond_broadcast(&cond_waiting_for_start);

            while (a_trolley -> passengers[j] -> action != MOVING &&
                   a_trolley -> passengers[j] -> action != MOVING_STARTED)
                pthread_cond_wait(&cond_passengers_preparing_to_start, &mutex);
        }

        print_message("Trolley nr. %d is starting moving.", a_trolley -> id);

        pop_queue(trolleys_queue, (void**) &object);
        push_queue(trolleys_queue, a_trolley);
        pthread_cond_broadcast(&cond_new_trolley_can_approach);

        pthread_mutex_unlock(&mutex);
        usleep(10000);
        pthread_mutex_lock(&mutex);

        print_message("Trolley nr. %d is ending moving.", a_trolley -> id);

        while (get_first_queue(trolleys_queue, (void**) &object) == QUEUE_SUCCESS && object != a_trolley)
            pthread_cond_wait(&cond_new_trolley_can_approach, &mutex);

        print_message("Trolley nr. %d is opening doors.", a_trolley -> id);

        while (a_trolley -> load != 0) {

            a_trolley -> passengers[a_trolley -> load - 1] -> action = READY_IN;
            pthread_cond_broadcast(&cond_passengers_waiting_to_leave);

            while (a_trolley -> passengers[a_trolley -> load - 1] -> action != INITIAL)
                pthread_cond_wait(&cond_passenger_left, &mutex);

            a_trolley -> passengers[a_trolley -> load - 1] -> a_trolley = NULL;
            a_trolley -> load--;
        }

        print_message("Trolley nr. %d is closing doors.", a_trolley -> id);
    }

    trolleys_left--;

    pop_queue(trolleys_queue, (void**) &object);
    pthread_cond_broadcast(&cond_new_trolley_can_approach);
    pthread_cond_broadcast(&cond_waiting_for_passenger_to_enter);

    print_message("Trolley nr. %d is closing it's thread.", a_trolley -> id);

    pthread_mutex_unlock(&mutex);
    return NULL;
}


int main(int argc, char* argv[]) {

    int passengers_count, trolleys_count;
    person** passengers;
    trolley** trolleys;

    if (argc < 5)
        die("Not enough arguments!\n");

    if (read_int_value(argv[1], &passengers_count) == -1 || read_int_value(argv[2], &trolleys_count) == -1 ||
        read_int_value(argv[3], &max_trolley_load) == -1 || read_int_value(argv[4], &rides_count) == -1)
        die("Invalid arguments!\n");

    srand((unsigned int) time(NULL));
    start_time = get_current_time();

    trolleys_left = trolleys_count;
    passengers = calloc((size_t) passengers_count, sizeof(person*));
    trolleys = calloc((size_t) passengers_count, sizeof(person*));

    passengers_queue = calloc(1, sizeof(queue));
    create_queue(passengers_queue, passengers_count);

    trolleys_queue = calloc(1, sizeof(queue));
    create_queue(trolleys_queue, trolleys_count);

    for (int i = 0; i < trolleys_count; i++) {

        trolleys[i] = calloc(1, sizeof(trolleys));
        trolleys[i] -> id = i;
        trolleys[i] -> load = 0;
        trolleys[i] -> passengers = calloc((size_t) max_trolley_load, sizeof(person*));

        push_queue(trolleys_queue, trolleys[i]);

        if (pthread_create(&trolleys[i] -> thread, NULL, trolley_loop, trolleys[i]) != 0)
            die("Creating trolley thread failed!\n");
    }

    for (int i = 0; i < passengers_count; i++) {

        passengers[i] = calloc(1, sizeof(person));
        passengers[i] -> id = i;
        passengers[i] -> action = INITIAL;

        push_queue(passengers_queue, passengers[i]);

        if (pthread_create(&passengers[i] -> thread, NULL, passenger_loop, passengers[i]) != 0)
            die("Creating passenger thread failed!\n");
    }

    for (int i = 0; i < trolleys_count; i++)
        pthread_join(trolleys[i] -> thread, NULL);

    for (int i = 0; i < passengers_count; i++)
        pthread_join(passengers[i] -> thread, NULL);

    destroy_queue(passengers_queue);
    free(passengers_queue);

    destroy_queue(trolleys_queue);
    free(trolleys_queue);

    for (int i = 0; i < trolleys_count; i++)
        free(trolleys[i]);

    free(trolleys);

    for (int i = 0; i < passengers_count; i++)
        free(passengers[i]);

    free(passengers);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&cond_waiting_for_passengers);
    pthread_cond_destroy(&cond_waiting_for_passenger_to_enter);
    pthread_cond_destroy(&cond_passenger_entering);
    pthread_cond_destroy(&cond_waiting_for_start);
    pthread_cond_destroy(&cond_passengers_preparing_to_start);
    pthread_cond_destroy(&cond_new_trolley_can_approach);
    pthread_cond_destroy(&cond_passengers_waiting_to_leave);
    pthread_cond_destroy(&cond_passenger_left);

    return 0;
}