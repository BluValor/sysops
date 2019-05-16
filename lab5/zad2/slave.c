#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>


int LINE_LENGHT = 100;


int main(int argc, char *argv[]) {

    srand((unsigned int) time(NULL));

    if (argc < 3) {

        fprintf(stderr, "Not enough arguments!\n");
        exit(1);
    }

    int named_pipe = open(argv[1], O_WRONLY);
    if (named_pipe < 0) {

        fprintf(stderr, "Error opening named pipe!\n");
        exit(1);
    }

    int pid = (int) getpid();
    printf("%d\n", pid);

    int number = (int) strtol(argv[2], NULL, 0);

    char date_str[LINE_LENGHT];

    for (int i = 0; i < number; i++) {

        printf("%d\n", i);

        FILE *date_file = popen("date", "r");
        fgets(date_str, LINE_LENGHT, date_file);

        char full_str[LINE_LENGHT + strlen(date_str)];
        if (sprintf(full_str, "%d -> %s", pid, date_str) < 0) {

            fprintf(stderr, "String concatenation failed!");
            exit(1);
        }

        if (write(named_pipe, full_str, strlen(full_str)) <  0) {

            fprintf(stderr, "Couldn't write to named pipe!");
            exit(1);
        }

        sleep((unsigned int) (rand() % 3 + 1));
    }

    close(named_pipe);
    return 0;
}
