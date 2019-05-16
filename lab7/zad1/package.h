#ifndef ZAD1_PACKAGE_H
#define ZAD1_PACKAGE_H


#include <sys/time.h>


typedef struct package {

    int weight;
    int pid;
    struct timeval timestamp;

} package;


package make_package(int weight);


#endif //ZAD1_PACKAGE_H
