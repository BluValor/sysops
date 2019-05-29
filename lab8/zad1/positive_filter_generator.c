#include <time.h>
#include <stdlib.h>
#include "filter.h"


int main() {

    srand((unsigned int) time(NULL));

    for (int i = 5; i <= 65; i += 5) {

        filter a_filter;
        new_filter(&a_filter, i);

        int sum = 0;

        for (int y = 0; y < i; y++) {

            for (int x = 0; x < i; x++) {

                int value;

                value = rand() % 100;
                sum += value;

                a_filter . v_ints[y][x] = value;
            }

        }

        a_filter . divider = sum;

        char path[64];
        sprintf(path, "./filters/random_pos_filter_%d.txt", i);

        save_filter(path, &a_filter);
        delete_filter(&a_filter);
    }
}

