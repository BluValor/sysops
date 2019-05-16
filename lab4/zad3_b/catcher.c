#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "shared_func.c"


int count = 0;


void c_kill_sigusr1(int signal, siginfo_t *info, void *param) {

    count++;
    kill(info -> si_pid, SIGUSR1);
}


void c_kill_sigusr2(int signal, siginfo_t *info, void *param) {

    kill(info -> si_pid, SIGUSR2);
    exit(0);
}


void c_queue_sigusr1(int signal, siginfo_t *info, void *param) {

    count++;
    union sigval value = {count};
    sigqueue(info -> si_pid, SIGUSR1, value);
}


void c_queue_sigusr2(int signal, siginfo_t *info, void *param) {

    kill(info->si_pid, SIGUSR2);
    exit(0);
}


void c_sigrt_min_0(int sig, siginfo_t *info, void *context) {

    count++;
    kill(info -> si_pid, SIGRTMIN);
}


void c_sigrt_min_1(int sig, siginfo_t *info, void *context) {

    kill(info -> si_pid, SIGRTMIN + 1);
    exit(0);
}


int main(int argc, char* argv[]) {

    if (argc - 1 < 1) {

        fprintf(stderr, "Not enough arguments.");
        exit(1);
    }

    char *mode = argv[1];

    if (strncmp(mode, "KILL", 4) == 0) {

        set_up_kill_queue(c_kill_sigusr1, c_kill_sigusr2);

    } else if (strncmp(mode, "SIGQUEUE", 8) == 0) {

        set_up_kill_queue(c_queue_sigusr1, c_queue_sigusr2);

    } else if (strncmp(mode, "SIGRT", 5) == 0) {

        set_up_sigrt(c_sigrt_min_0, c_sigrt_min_1);

    }  else {

        fprintf(stderr, "Wrong argument.");
        exit(1);
    }

    printf("PID: %d\n", (int) getpid());

    while (1)
        pause();

    return 0;
}
