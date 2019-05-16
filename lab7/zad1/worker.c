#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include "exit_func.h"
#include "ipc_func.h"
#include "shared_func.h"
#include "cbelt.h"


int package_weight;
long sem_id;
cbelt* a_cbelt;
int prev_result = 0;


void cleanup() {

    sem_m_close(sem_id);
    shm_m_detach(a_cbelt, CBELT_SIZE);
}


void sigint_handler(int signal) {

    exit(0);
}


void put_package() {

    package a_package = make_package(package_weight);
    int result = cbelt_put(a_cbelt, sem_id, a_package);

    if (result == 0) {

        printf("Worker %10d put %d kg package on the conveyor belt. Timestamp: %ld\n", getpid(), package_weight,
               a_package.timestamp.tv_sec * 1000000 + a_package.timestamp.tv_usec);

    } else if (result == -1 && prev_result != -1) {

        printf("Worker %10d tried putting package on the conveyor belt, but it's maximum weight is reached!\n", getpid());

    } else if (result == -1 && prev_result != -1) {

        printf("Worker %10d tried putting package on the conveyor belt, but there is no space left!\n", getpid());
    }

    prev_result = result;
}


int main(int argc, char* argv[]) {

    long loops = 0;

    if (argc == 2) {

        long tmp = strtol(argv[1], NULL, 0);

        if (tmp == 0 || tmp >= INT_MAX || tmp <= INT_MIN)
            die("Invalid argument!\n");

        package_weight = (int) tmp;

    } else if (argc == 3) {

        long tmp1 = strtol(argv[1], NULL, 0);
        long tmp2 = strtol(argv[2], NULL, 0);

        if (tmp1 == 0 || tmp1 >= INT_MAX || tmp1 <= INT_MIN ||
            tmp2 == 0 || tmp2 >= INT_MAX || tmp2 <= INT_MIN)
            die("Invalid arguments!\n");

        package_weight = (int) tmp1;
        loops = (int) tmp2;

    } else die("Not enough arguments! Required arguments: package_weight, loops (optional).\n");

    atexit(cleanup);
    signal(SIGINT, sigint_handler);

    long shm_id = shm_m_open(get_truck_key(), CBELT_SIZE);
    a_cbelt = shm_m_attach(shm_id, CBELT_SIZE);
    sem_id = sem_m_open(a_cbelt -> sem_key);

    if (!loops) {

        while (1) {

            put_package();
            usleep(1000);
        }

    } else {

        for (long i = 0; i < loops; i++) {

            put_package();
            usleep(1000);
        }

        return 0;
    }
}