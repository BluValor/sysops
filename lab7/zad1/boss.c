#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include "exit_func.h"


int main(int argc, char* argv[]) {

    srand((unsigned int) time(NULL));

    if (argc != 3)
        die("Not enough arguments! Required arguments: worker_nr, max_package_weight.\n");

    long tmp1 = strtol(argv[1], NULL, 0);
    long tmp2 = strtol(argv[2], NULL, 0);

    if (tmp1 == 0 || tmp1 >= INT_MAX || tmp1 <= INT_MIN ||
        tmp2 == 0 || tmp2 >= INT_MAX || tmp2 <= INT_MIN)
        die("Invalid arguments!\n");

    int worker_nr = (int) tmp1;
    int max_package_weight = (int) tmp2;

    char single_weight[16];

    for (int i = 0; i < worker_nr; i++) {

        sprintf(single_weight, "%d", rand() % max_package_weight + 1);

        pid_t cpid = fork();

        if (cpid == 0) {

            execl("./worker", "./worker", single_weight, NULL);
            die_errno("Exec failure!\n");

        } else if (cpid == -1)
            die_errno("Fork failure!\n");

    }

    int worker_status;

    for (int i = 0; i < worker_nr; i++) {

        wait(&worker_status);
        printf("Worker nr. %d exit status: %d.\n", i, WEXITSTATUS(worker_status));
    }

    return 0;
}