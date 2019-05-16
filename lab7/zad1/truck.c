#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include "exit_func.h"
#include "ipc_func.h"
#include "shared_func.h"
#include "cbelt.h"


long sem_id, shm_id;
int truck_capacity, cbelt_size, cbelt_capacity;
cbelt* a_cbelt;


void cleanup() {

    sem_m_destroy(sem_id, get_cbelt_key());
    shm_m_detach(a_cbelt, CBELT_SIZE);
    shm_m_destroy(shm_id, get_truck_key());
}


void sigint_handler(int signal) {

    exit(0);
}


int main(int argc, char* argv[]) {

    if (argc != 4)
        die("Not enough arguments! Required arguments: truck_capacity, cbelt_size, cbelt_capacity.\n");

    long tmp1 = strtol(argv[1], NULL, 0);
    long tmp2 = strtol(argv[2], NULL, 0);
    long tmp3 = strtol(argv[3], NULL, 0);

    if (tmp1 == 0 || tmp1 >= INT_MAX || tmp1 <= INT_MIN ||
        tmp2 == 0 || tmp2 >= INT_MAX || tmp2 <= INT_MIN ||
        tmp3 == 0 || tmp3 >= INT_MAX || tmp3 <= INT_MIN)
        die("Invalid arguments!\n");

    truck_capacity = (int) tmp1;
    cbelt_size = (int) tmp2;
    cbelt_capacity = (int) tmp2;

    atexit(cleanup);
    signal(SIGINT, sigint_handler);

    shm_id = shm_m_create(get_truck_key(), CBELT_SIZE);
    a_cbelt = shm_m_attach(shm_id, CBELT_SIZE);
    sem_id = sem_m_create(get_cbelt_key());
    *a_cbelt = make_cbelt(cbelt_size, cbelt_capacity, get_cbelt_key());

    int curr_weight = 0;

    package a_package;
    struct timeval curr_time;

    printf("New truck arrived!\n");
    long cbelt_empty = 0;

    while (1) {

        usleep(300);

        if (cbelt_take(a_cbelt, sem_id, &a_package) == NULL) {

            if (cbelt_empty == 0)
                printf("There is nothing on conveyor belt!\n");

            cbelt_empty = 1;
            continue;
        }

        cbelt_empty = 0;
        gettimeofday(&curr_time, NULL);

        if (curr_weight + a_package . weight > truck_capacity) {

            sem_m_block(sem_id);

            printf("Truck filled up! Unloading truck...\n");
            sleep(1);

            curr_weight = 0;
            printf("New truck arrived!");

            sem_m_unblock(sem_id);
        }

        if (a_package . weight > truck_capacity)
            die("Package is too heavy!\n");

        curr_weight += a_package . weight;

        printf("Package is being loaded:\npid: %12d, time: %12ld us, current load: %6d kg, space left: %6d kg\n",
                a_package . pid, ((curr_time . tv_sec - a_package . timestamp . tv_sec) * 1000000) +
                                 (curr_time . tv_usec - a_package . timestamp . tv_usec), curr_weight,
                                 truck_capacity - curr_weight);
    }
}