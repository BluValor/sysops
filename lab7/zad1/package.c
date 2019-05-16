#include <unistd.h>
#include "package.h"


package make_package(int weight) {

    package a_package;
    a_package . weight = weight;
    a_package . pid = getpid();
    gettimeofday(&a_package . timestamp, NULL);

    return a_package;
}