#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include "exit_func.h"
#include "image.h"
#include "filter.h"
#include "shared_func.h"


#define BLOCK 0
#define INTERLEAVED 1


static int threads_count, sharing_type;
static char *input_path, *filter_path, *output_path;
image input_image, output_image;
filter a_filter;


void count_time_diff(struct timeval* buffer, struct timeval* start, struct timeval* end) {

    if ((end -> tv_usec - start -> tv_usec) < 0) {

        buffer -> tv_sec = end -> tv_sec - start -> tv_sec - 1;
        buffer -> tv_usec = 1000000 + end -> tv_usec - start -> tv_usec;

    } else {

        buffer -> tv_sec = end -> tv_sec - start -> tv_sec;
        buffer -> tv_usec = end -> tv_usec - start -> tv_usec;
    }
}


void *thread_routine(void *number_pointer) {

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    int* number = (int*) number_pointer;

    if (sharing_type == BLOCK) {

        int n = input_image . width;
        int const_value = (n % threads_count == 0 ? n / threads_count : n / threads_count + 1);

        int x_left = (*number - 1) * const_value;
        int x_right = *number * const_value - 1;
        x_right = (x_right < input_image . width ? x_right : input_image.width - 1);

        for (int y = 0; y < input_image . height; y++)
            for (int x = x_left; x <= x_right; x++)
                output_image . values[y][x] = calc_for_value(&input_image, &a_filter, x, y);

    } else {

        int x = *number - 1;

        for (; x < input_image . width; x += threads_count)
            for (int y = 0; y < input_image . height; y++)
                output_image . values[y][x] = calc_for_value(&input_image, &a_filter, x, y);
    }

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    struct timeval* time_difference = malloc(sizeof(struct timeval));
    count_time_diff(time_difference, &start_time, &end_time);

    pthread_exit(time_difference);
}


int main(int argc, char* argv[]) {

    if (argc < 6)
        die("Not enough arguments!\n");

    if ((read_int_value(argv[1], &threads_count)) == -1 || threads_count < 1)
        die("Invalid threads count!\n");

    if ((read_int_value(argv[2], &sharing_type)) == -1 || (sharing_type != 0 && sharing_type != 1))
        die("Invalid sharing type!\n");

    input_path = argv[3];
    filter_path = argv[4];
    output_path = argv[5];

    printf("Input: %d %d %s %s %s\n", threads_count, sharing_type, input_path, filter_path, output_path);

    load_image(input_path, &input_image);
    new_image(&output_image, input_image . width, input_image . height);
    load_filter(filter_path, &a_filter);

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    pthread_t* threads = malloc(threads_count * sizeof(pthread_t));
    int *number_pointers = malloc(threads_count * sizeof(int));

    for (int i = 0; i < threads_count; i++) {

        number_pointers[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_routine, &number_pointers[i]);
    }

    struct timeval *return_value;

    for (int i = 0; i < threads_count; i++) {

        pthread_join(threads[i], (void **) &return_value);
        printf("Thread nr. %d (%ld) returned in %ld s %ld us.\n", i + 1, threads[i], return_value -> tv_sec,
                return_value -> tv_usec);
        free(return_value);
    }

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    struct timeval time_difference;
    count_time_diff(&time_difference, &start_time, &end_time);

    printf("Filtering done in %ld s %ld us.\n", time_difference . tv_sec, time_difference . tv_usec);

    FILE* file = fopen("./Times.txt", "a");
    if (file == NULL)
        die("Opening report file failed!\n");

    fprintf(file, "%-14s filter size %-5d (%d threads): %10ld s %15ld us\n",
            (sharing_type == BLOCK ? "block" : "interleaved"), a_filter . size, threads_count,
            time_difference . tv_sec, time_difference . tv_usec);

    fclose(file);

    save_image(output_path, &output_image);

    free(threads);
    free(number_pointers);
    delete_image(&input_image);
    delete_image(&output_image);
    delete_filter(&a_filter);

    return 0;
}