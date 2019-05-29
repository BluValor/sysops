#ifndef SYSOPY_LAB_9_EXIT_FUNC_H
#define SYSOPY_LAB_9_EXIT_FUNC_H


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


void die(const char *err_template, ...);
void die_errno(const char *err_template, ...);


#endif //SYSOPY_LAB_9_EXIT_FUNC_H
