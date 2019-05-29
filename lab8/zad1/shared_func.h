#ifndef ZAD1_SHARED_FUNC_H
#define ZAD1_SHARED_FUNC_H


#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "image.h"
#include "filter.h"


int read_int_value(const char* input, int *buffer);
int calc_for_value(image* a_image, filter* a_filter, int x, int y);


#endif //ZAD1_SHARED_FUNC_H
