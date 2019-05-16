#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>


int MAX_LINE_CMD_NR = 20;
int MAX_ARG_NR = 20;


typedef struct {

    char *name;
    char **argv;
    int argc;

} command;


typedef struct {

    command *list;
    int size;

} command_list;


char* read_nth_line(char *path, int n) {

    char *line = NULL;
    size_t len = 0;

    FILE *file = fopen(path, "r");
    if (file == NULL) {

        fprintf(stderr, "Couldn't open file %s!\n", path);
        exit(EXIT_FAILURE);
    }

    int i = -1;
    while (getline(&line, &len, file) >= 0 && ++i != n) {}

    if (fclose(file) != 0) {

        fprintf(stderr, "Couldn't close file %s!\n", path);
        exit(EXIT_FAILURE);
    }

    if (i != n) {

        free(line);
        return NULL;
    }

    return strtok(line, "\n");
}


command parse_single_command(char *source) {

    char *saveptr;
    command cmd;

    char **argv = malloc((MAX_ARG_NR + 2) * sizeof(char*));
    if (!argv) {

        fprintf(stderr, "Couldn't allocate memory for arguments list!\n");
        exit(EXIT_FAILURE);
    }

    char *arg = strtok_r(source, " ", &saveptr);
    cmd . name = arg;
    char *tmp = malloc(strlen(arg) + 1);
    strcpy(tmp, arg);
    strcat(tmp, "\0");
    argv[0] = tmp;
    arg = strtok_r(NULL, " ", &saveptr);

    int argc;
    for (argc = 1;  arg != NULL; argc++, arg = strtok_r(NULL, " ", &saveptr)) {

        if (argc < MAX_ARG_NR) {

            char *tmp = malloc(strlen(arg) + 1);
            strcpy(tmp, arg);
            strcat(tmp, "\0");
            argv[argc] = tmp;
        }

        else {

            fprintf(stderr, "Argument limit exceeded!\n");
            exit(EXIT_FAILURE);
        }
    }

    argv[argc] = NULL;
    cmd . argv = argv;
    cmd . argc = argc;
    return cmd;
}


command_list parse_command_line(char *line) {

    char *saveptr;
    command_list cmd_list;

    command *list = malloc((MAX_LINE_CMD_NR) * sizeof(command));
    if (!list) {

        fprintf(stderr, "Couldn't allocate memory for command list!\n");
        exit(EXIT_FAILURE);
    }

    char *single_command_str = strtok_r(line, "|", &saveptr);

    int size;
    for (size = 0; single_command_str != NULL; size++, single_command_str = strtok_r(NULL, "|", &saveptr)) {

        if (size < MAX_LINE_CMD_NR)
            list[size] = parse_single_command(single_command_str);

        else {

            fprintf(stderr, "Command in line limit exceeded!\n");
            exit(EXIT_FAILURE);
        }
    }

    cmd_list . list = list;
    cmd_list . size = size;
    return cmd_list;
}


void run_commands(command_list cmd_list) {

    int fds[2][2];

    int i;
    for (i = 0; i < cmd_list . size; i++) {

        if (i > 0) {

            close(fds[i % 2][0]);
            close(fds[i % 2][1]);
        }

        if (pipe(fds[i % 2]) < 0) {

            fprintf(stderr, "pipe setup error!");
            exit(1);
        }

        command cmd = cmd_list.list[i];

        pid_t cpid = fork();

        if (cpid == 0) {

            if (i != cmd_list . size - 1) {

                close(fds[i % 2][0]);

                if (dup2(fds[i % 2][1], STDOUT_FILENO) < 0) {

                    fprintf(stderr, "stdout setup error!");
                    exit(1);
                }
            }

            if (i != 0) {

                close(fds[(i + 1) % 2][1]);

                if (dup2(fds[(i + 1) % 2][0], STDIN_FILENO) < 0){

                    fprintf(stderr, "stdin setup error!");
                    exit(1);
                }
            }

            execvp(cmd . name, cmd . argv);

        }
    }

    close(fds[i % 2][0]);
    close(fds[i % 2][1]);
}


int main(int argc, char *argv[]) {

    if (argc < 2) {

        fprintf(stderr, "Not enough arguments!");
        exit(EXIT_FAILURE);
    }

    char *path = argv[1];

    int i = 0;
    char *buffer = read_nth_line(path, i);

    while (buffer != NULL) {

        i++;

        command_list cmd_line = parse_command_line(buffer);
        run_commands(cmd_line);

        for (int j = 0; j < cmd_line . size; j++)
            wait(NULL);

        buffer = read_nth_line(path, i);
    }

    free(buffer);

    return 0;
}
