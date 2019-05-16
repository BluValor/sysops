#include "exit_func.h"


void die(const char *err_template, ...) {

    va_list args;
    va_start(args, err_template);
    vfprintf(stderr, err_template, args);
    va_end(args);
    fputs("\n", stderr);

    exit(EXIT_FAILURE);
}


void die_errno(const char *err_template, ...) {

    va_list args;
    va_start(args, err_template);
    vfprintf(stderr, err_template, args);
    va_end(args);
    fprintf(stderr, " -> %s\n", strerror(errno));

    exit(EXIT_FAILURE);
}

