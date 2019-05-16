#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


int LINE_LENGHT = 1024;


int main(int argc, char *argv[]) {

    if (argc < 2) {

        fprintf(stderr, "Not enough arguments!\n");
        exit(1);
    }

    char *path = argv[1];

    char line[LINE_LENGHT];

    if (mkfifo(path, S_IRUSR | S_IWUSR) < 0) {

        fprintf(stderr, "fifo setup error!\n");
        exit(1);
    }

    FILE *pipe_file = fopen(path, "r");
    if (pipe_file == NULL) {

        fprintf(stderr, "Couldn't open file %s!\n", path);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, LINE_LENGHT, pipe_file) != NULL)
        printf("%s", line);

    fclose(pipe_file);

    return 0;
}