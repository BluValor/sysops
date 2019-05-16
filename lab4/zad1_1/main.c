#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


int mode = 0;


void sigint_handler(int signal) {

    printf("\rSIGINT signal received.\n");
    exit(1);
}


void sigtstp_handler(int signal) {

    if (mode == 0){

        mode = 1;
        printf(" Waiting for CTRL+Z - continuation or CTR+C - end program.\n");

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGTSTP);
        sigdelset(&mask, SIGINT);
        sigsuspend(&mask);

    } else {

        mode = 0;
    }
}


int main() {

    struct sigaction sigtstp_act;
    sigtstp_act . sa_handler = sigtstp_handler;
    sigemptyset(&sigtstp_act . sa_mask);
    sigtstp_act.sa_flags = 0;

    if (sigaction(SIGTSTP, &sigtstp_act, NULL)) {

        fprintf(stderr, "Error occured during SIGTSTP catch setup.");
        exit(1);
    }

    if (signal(SIGINT, &sigint_handler) == SIG_ERR) {

        fprintf(stderr, "Error occured during SIGINT catch setup.");
        exit(1);
    }

    while(1) {

        system("date");
        sleep(1);
    }

    return 0;
}