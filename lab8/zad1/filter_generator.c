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

                if (rand() % 5 == 0) {

                    value = -sum + 1;
                    sum += value;

                } else {

                    value = rand() % 100;
                    value = (rand() % 2 == 0 ? value : -value);
                    sum += value;
                }

                a_filter . v_ints[y][x] = value;
            }

        }

        a_filter . v_ints[rand() % i][rand() % i] = -sum + 1;

        char path[64];
        sprintf(path, "./filters/random_filter_%d.txt", i);

        save_filter(path, &a_filter);
        delete_filter(&a_filter);
    }
}

