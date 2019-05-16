#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void set_up_kill_queue(void(*fun1) (int, siginfo_t*, void*), void(*fun2) (int, siginfo_t*, void*)) {

    sigset_t signals;
    sigfillset(&signals);
    sigdelset(&signals, SIGUSR1);
    sigdelset(&signals, SIGUSR2);
    sigprocmask(SIG_SETMASK, &signals, NULL);

    struct sigaction act;
    sigfillset(&signals);
    act . sa_mask = signals;
    act . sa_flags = SA_SIGINFO;

    act.sa_sigaction = fun1;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = fun2;
    sigaction(SIGUSR2, &act, NULL);
}


void set_up_sigrt(void(*fun1) (int, siginfo_t*, void*), void(*fun2) (int, siginfo_t*, void*)) {

    sigset_t signals;
    sigfillset(&signals);
    sigdelset(&signals, SIGRTMIN);
    sigdelset(&signals, SIGRTMIN + 1);
    sigprocmask(SIG_SETMASK, &signals, NULL);

    struct sigaction act;
    sigfillset(&signals);
    act . sa_mask = signals;
    act . sa_flags = SA_SIGINFO;

    act.sa_sigaction = fun1;
    sigaction(SIGRTMIN, &act, NULL);

    act.sa_sigaction = fun2;
    sigaction(SIGRTMIN + 1, &act, NULL);
}
