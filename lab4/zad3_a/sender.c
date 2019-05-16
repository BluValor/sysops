#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "shared_func.c"


int sig_sent;
int sig_received;


void s_kill_sigusr1(int signal, siginfo_t *info, void *param) {

    sig_received++;
}


void s_kill_sigusr2(int signal, siginfo_t *info, void *param) {

    printf("Signals sent: %d.\nSignals received: %d.\n", sig_sent, sig_received);
    exit(0);
}


void s_queue_sigusr1(int sig, siginfo_t *info, void *param) {

    sig_received++;
    printf("SIGUSR1 nr. %d received.\n", info -> si_value . sival_int);
}


void s_queue_sigusr2(int sig, siginfo_t *info, void *param) {

    printf("Signals sent: %d.\nSignals received: %d.\n", sig_sent, sig_received);
    exit(0);
}


void s_sigrt_min_0(int sig, siginfo_t *info, void *context) {

    sig_received++;
}


void s_sigrt_min_1(int sig, siginfo_t *info, void *context) {

    printf("Signals sent: %d.\nSignals received: %d.\n", sig_sent, sig_received);
    exit(0);
}


int main(int argc, char* argv[]) {

    if (argc - 1 < 3) {

        fprintf(stderr, "Not enough arguments.");
        exit(1);
    }

    int catcher_pid = (int) strtol(argv[1], NULL, 0);
    sig_sent = (int) strtol(argv[2], NULL, 0);
    char *mode = argv[3];

    if (strncmp(mode, "KILL", 4) == 0) {

        set_up_kill_queue(s_kill_sigusr1, s_kill_sigusr2);

        for (int i = 0; i < sig_sent; i++) {

            kill(catcher_pid, SIGUSR1);
        }

        kill(catcher_pid, SIGUSR2);

    } else if (strncmp(mode, "SIGQUEUE", 8) == 0) {

        set_up_kill_queue(s_queue_sigusr1, s_queue_sigusr2);

        for (int i = 0; i < sig_sent; i++) {

            union sigval value = {i};
            sigqueue(catcher_pid, SIGUSR1, value);
        }

        union sigval value = {sig_received};
        sigqueue(catcher_pid, SIGUSR2, value);

    } else if (strncmp(mode, "SIGRT", 5) == 0) {

        set_up_sigrt(s_sigrt_min_0, s_sigrt_min_1);

        for (int i = 0; i < sig_sent; i++) {

            kill(catcher_pid, SIGRTMIN);
        }

        kill(catcher_pid, SIGRTMIN + 1);

    } else {

        fprintf(stderr, "Wrong argument.");
        exit(1);
    }

    while (1)
        pause();
    return 0;
}