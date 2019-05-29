// return -1 if failure, positive value instead


#include "shared_func.h"


int read_int_value(const char* input, int *buffer) {

    char *end = NULL;
    errno = 0;
    long result = strtol(input, &end, 10);
    if (result >= INT_MAX || end == input || errno != 0)
        return -1;
    *buffer = (int) result;
    return 0;
}


int calc_for_value(image* a_image, filter* a_filter, int x, int y) {

    float result = 0.0;
    int c = a_filter -> size;

    float const_x = x - ceilf((float) (c / 2.0));
    float const_y = y - ceilf((float) (c / 2.0));


    for (int cy = 1; cy <= c; cy++)
        for (int cx = 1; cx <= c; cx++)
            result += a_image->values[(int) fminf(a_image -> height, fmaxf(1.0, const_y + cy)) - 1]
                    [(int) fminf(a_image -> width, fmaxf(1.0, const_x + cx)) - 1] * a_filter->v_floats[cy - 1][cx - 1];

    return abs((int) roundf(result));
}