#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


int mode = 0;
pid_t child_proc;


void start_loop() {

    pid_t child = fork();

    if (child == 0) {

        execl("./loop_date.sh", "./loop_date.sh", NULL);
        fprintf(stderr, "Could't start loop_date.sh.\n");
        exit(1);

    } else
        child_proc = child;
}


void sigint_handler(int signal) {

    printf("\rSIGINT signal received.\n");
    if (mode == 0)
        kill(child_proc, SIGINT);
    exit(1);
}


void sigtstp_handler(int signal) {

    if (mode == 0){

        printf("\rWaiting for CTRL+Z - continuation or CTR+C - end program.\n");
        kill(child_proc, SIGINT);
        mode = 1;

    } else {

        start_loop();
        mode = 0;
    }
}


int main() {

    struct sigaction sigtstp_act;
    sigtstp_act . sa_handler = sigtstp_handler;
    sigemptyset(&sigtstp_act . sa_mask);
    sigtstp_act.sa_flags = 0;

    if (sigaction(SIGTSTP, &sigtstp_act, NULL)) {

        fprintf(stderr, "Error occured during SIGTSTP catch setup.\n");
        exit(1);
    }

    if (signal(SIGINT, &sigint_handler) == SIG_ERR) {

        fprintf(stderr, "Error occured during SIGINT catch setup.\n");
        exit(1);
    }

    start_loop();

    while(1)
        pause();

    return 0;
}