#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


#define TIME_PATTERN "%F_%H-%M-%S "


int number_of_digits(int number) {

    int result = 0;

    while(number > 0) {

        number /= 10;
        result++;
    }

    return result;
}


char *format_time(time_t time) {

    char *buf = malloc(22 * sizeof (char));
    strftime(buf, 22, TIME_PATTERN, localtime(&time));

    buf[22] = '\0';

    return buf;
}


int main(int argc, char* argv[]) {

    srand((unsigned int) time(NULL));

    if (argc < 5){

        fprintf(stderr, "Not enough arguments.");
        return -1;
    }

    char* file_path = argv[1];

    int pmin = (int) strtol(argv[2], NULL, 0);
    if (pmin < 0) {

        fprintf(stderr,"pmin - bad value: %d", pmin);
        return -1;
    }

    int pmax = (int) strtol(argv[3], NULL, 0);
    if (pmax < 0) {

        fprintf(stderr,"pmax - bad value: %d", pmax);
        return -1;
    }

    int bytes = (int) strtol(argv[4], NULL, 0);
    if (bytes < 0) {

        fprintf(stderr,"bytes - bad value: %d", bytes);
        return -1;
    }


    for (int i = 0; i < 10; i++){

        int rand_delay = rand() % (pmax - pmin + 1) + pmin;
        int size = 53 + number_of_digits(getpid()) + number_of_digits(rand_delay);

        char* text_line = malloc((size_t) size);

        sprintf(text_line, "delay: %d ", rand_delay);

        int pid_size = number_of_digits(getpid());
        char* tmp_pid = malloc((size_t) pid_size);
        sprintf(tmp_pid, "| pid_number: %d ", getpid());
        strcat(text_line, tmp_pid);

        strcat(text_line, "| time: ");

        time_t raw_time = time(NULL);
        char* curr_time = format_time(raw_time);
        strcat(text_line, curr_time);

        strcat(text_line, "| ");

        FILE *file = fopen(file_path, "a+");

        if (!file) {

            fprintf(stderr, "Failed accessing file: %s.", file_path);
            return -1;
        }

        if (fwrite(text_line, sizeof(char), (size_t) size, file)!=size){

            fprintf(stderr,"Failed writing to file: %s", file_path);
            return -1;
        }


        for (int j = 0; j < bytes; j++) {

            int rand_char = (char) (97 + rand() % 26);
            putc(rand_char, file);
        }

        putc('\n', file);

        fclose(file);
        printf("%s\n", text_line);

        sleep(rand_delay);
    }

    return 0;
}