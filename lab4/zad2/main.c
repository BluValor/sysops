#define TIME_PATTERN "_%F_%H-%M-%S_"
#define ARCHIVE_PATH "../archive/"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <ftw.h>
#include <limits.h>
#include <wait.h>
#include <math.h>

//./list.txt

struct parent_property p_props;
struct child_property c_props;


struct parent_property {

    pid_t *proc_pids;
    int lines_nr;
};


struct child_property {

    int mode;
    char *file_content;
    int modified_files;
};


/** returns NULL if cannot open given file or if given line_nr is too big **/
char* read_line(char *file_path, int line_nr) {

    FILE *file;
    file = fopen(file_path, "r");

    if (!file) {

        fprintf(stderr, "cannot open file: %s", file_path);
        return NULL;
    }

    char *buffer = NULL;
    size_t line_length = 1024;

    for (int i = 0; i <= line_nr; i++)

        if ((int) getline(&buffer, &line_length,  file) == -1) {

            fprintf(stderr, "error in function read_line (too big line_nr: %d)", line_nr);
            return NULL;
        }

    fclose(file);
    return buffer;
}


/** returns -1 if cannot open given file **/
int count_lines(char *file_path) {

    FILE *file = NULL;
    file = fopen(file_path, "r");

    if (!file) {

        fprintf(stderr, "cannot open file: %s", file_path);
        return -1;
    }

    char *buffer = NULL;
    size_t line_length = 1024;
    int curr_line = 0;

    while ((int) getline(&buffer, &line_length, file) != -1)
        curr_line++;

    fclose(file);
    return curr_line;
}


char* read_till_delim(char *text, int word_nr, char delim) {

    int index = 0;
    int delim_count = 0;
    int prev_index = 0;

    while (text[index] != '\0') {

        index++;

        if (text[index] == delim || text[index] == '\0') {

            delim_count++;

            if (delim_count == word_nr)
                break;

            prev_index = index;
        }
    }

    char *buffer = malloc((index - prev_index) * sizeof(char));

    if (prev_index == 0)
        memcpy(buffer, text + prev_index, (size_t) (index - prev_index));
    else
        memcpy(buffer, text + prev_index + 1, (size_t) (index - prev_index - 1));


    buffer[index - prev_index] = '\0';

    return buffer;
}


long get_file_size(char *file_path) {

    FILE *file = NULL;
    file = fopen(file_path, "r");

    long size = 0;
    fseek(file, 0, 2);
    size = ftell(file);
    fclose(file);

    return size;
}


/** returns NULL if cannot open given file **/
char *get_file_content(char *file_path) {

    FILE *file = NULL;
    file = fopen(file_path, "r");

    if (!file) {

        fprintf(stderr, "cannot open file: %s", file_path);
        return NULL;
    }

    struct stat stats;
    stat(file_path, &stats);
    char *buffer = malloc((size_t) (stats . st_size + 1));
    size_t size = (size_t) get_file_size(file_path);

    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}


char *format_time(time_t time) {

    char *buf = malloc(21 * sizeof (char));
    strftime(buf, 21, TIME_PATTERN, localtime(&time));

    buf[20] = '\0';

    return buf;
}


int make_copy(char *file_path, char *file_name, char *file_content, size_t prev_size, time_t prev_mod) {

    char *copy_path = malloc(256 * sizeof (char));
    sprintf(copy_path, "%s%s%s.txt", ARCHIVE_PATH, file_name, format_time(prev_mod));

    FILE *file = fopen(copy_path, "w");

    if (!file) {

        fprintf(stderr, "Making copy of a file: %s failed.", file_path);
        return -1;
    }

    fwrite(file_content, sizeof (char), prev_size, file);

    fclose(file);
    free(copy_path);
    return 0;
}


int count_digits(int value) {

    int count = 0;
    int tmp = value;

    while (tmp > 0) {

        tmp /= 10;
        count++;
    }

    return count;
}


void child_sigint_handler(int signal) {

    free(c_props . file_content);
    exit(c_props . modified_files);
}


void child_sigusr1_handler(int signal) {

    printf("sigusr1\n");
    c_props . mode = 1;
}


void child_sigusr2_handler(int signal) {

    printf("sigusr2\n");
    c_props . mode = 0;
}


void parent_sigint_handler(int signal) {

    for (int i = 0; i < p_props . lines_nr; i++) {

        kill(p_props . proc_pids[i], SIGINT);
        int c_exit_status;
        waitpid(p_props . proc_pids[i], &c_exit_status, 0);
        printf("Process %d made %d copies of file.\n", p_props . proc_pids[i], WEXITSTATUS(c_exit_status));
    }

    free(p_props . proc_pids);
    exit(0);
}


int memory_cpy(char *file_path, char *file_name, int interval) {

    c_props . mode = 0;
    c_props . modified_files = 0;

    size_t file_size = (size_t) get_file_size(file_path);
    c_props . file_content = get_file_content(file_path);

    if (!c_props . file_content)
        exit(c_props . modified_files);

    struct stat stats;
    stat(file_path, &stats);
    time_t prev_mod = stats . st_mtime;

    while (1) {

        sleep((unsigned int) interval);

        stat(file_path, &stats);

        if (prev_mod == stats . st_mtime || c_props . mode)
            continue;

        prev_mod = stats . st_mtime;

        if (make_copy(file_path, file_name, c_props . file_content, file_size, prev_mod) < 0) {

            free(c_props . file_content);
            exit(c_props . modified_files);
        }

        free(c_props . file_content);
        file_size = (size_t) get_file_size(file_path);
        c_props . file_content = get_file_content(file_path);

        if (!c_props . file_content)
            exit(c_props . modified_files);

        c_props . modified_files++;
    }

    free(c_props . file_content);
    return c_props . modified_files;
}


void set_up_child() {

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    struct sigaction act;
    sigfillset(&act . sa_mask);
    act . sa_flags = 0;

    act . sa_handler = child_sigint_handler;
    sigaction(SIGINT, &act, NULL);

    act . sa_handler = child_sigusr1_handler;
    sigaction(SIGUSR1, &act, NULL);

    act . sa_handler = child_sigusr2_handler;
    sigaction(SIGUSR2, &act, NULL);
}


pid_t* monitor_start_childs(char *list_path, int lines_nr) {

    pid_t *proc_pids = malloc(lines_nr * sizeof(pid_t));

    for (int i = 0; i < lines_nr; i++) {

        char *line = read_line(list_path, i);

        char *file_name = read_till_delim(line, 1, ' ');
        char *file_path = realpath(read_till_delim(line, 2, ' '), NULL);

        if (!list_path) {

            fprintf(stderr, "Wrong path: %s\n", read_till_delim(line, 2, ' '));
            exit(1);
        }

        char *tmp = read_till_delim(line, 3, ' ');
        int interval = (int) strtol(tmp, NULL, 0);

        free(tmp);
        if (interval < 0) {

            fprintf(stderr, "Wrong time interval: %d, for file: %s.\n", interval, file_name);
            free(file_name);
            free(file_path);
            exit(1);
        }
        free(line);

        int child_pid = fork();
        int modified_files = 0;

        if (child_pid == 0) {

            set_up_child();
            modified_files = memory_cpy(file_path, file_name, interval);
            _exit(modified_files);

        } else {

            printf(" -> %7d -> %s\n", child_pid, file_name);
            proc_pids[i] = child_pid;
        }

        free(file_name);
        free(file_path);
    }

    return proc_pids;
}


int monitor(char *list_path) {

    if ((p_props . lines_nr = count_lines(list_path)) == -1)
        return -1;

    p_props . proc_pids = monitor_start_childs(list_path, p_props . lines_nr);

    while (1) {

        char command[20];
        puts("Enter command: ");
        fgets(command, sizeof(command), stdin);
        printf("%s\n", command);

        if (strncmp(command, "LIST", 4) == 0) {

            for (int i = 0; i < p_props . lines_nr; i++) {

                printf(" -> %7d\n", p_props . proc_pids[i]);
            }

            printf("\n");
        }

        if (strncmp(command, "STOP PID", 4) == 0) {

            for (int i = 0; i < p_props . lines_nr; i++) {

                char pid_str[21];
                sprintf(pid_str, "%d", p_props . proc_pids[i]);

                if (strncmp(command + 5, pid_str, (size_t) count_digits(p_props . proc_pids[i])) == 0) {

                    kill(p_props . proc_pids[i], SIGUSR1);
                }
            }
        }

        if (strncmp(command, "STOP ALL", 8) == 0) {

            for (int i = 0; i < p_props . lines_nr; i++) {

                kill(p_props . proc_pids[i], SIGUSR1);
            }
        }

        if (strncmp(command, "START", 5) == 0) {

            for (int i = 0; i < p_props . lines_nr; i++) {

                char pid_str[21];
                sprintf(pid_str, "%d", p_props . proc_pids[i]);

                if (strncmp(command + 6, pid_str, (size_t) count_digits(p_props . proc_pids[i])) == 0) {

                    kill(p_props . proc_pids[i], SIGUSR2);
                }
            }
        }

        if (strncmp(command, "START ALL", 9) == 0) {

            for (int i = 0; i < p_props . lines_nr; i++) {

                kill(p_props . proc_pids[i], SIGUSR2);
            }
        }


        if (strncmp(command, "END", 3) == 0) {

            kill(getpid(), SIGINT);
        }
    }

    return 0;
}


int main(int argc, char *argv[]) {

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct sigaction act;
    act . sa_handler = parent_sigint_handler;
    sigfillset(&act . sa_mask);
    act . sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    if (argc - 1 < 1) {

        fprintf(stderr, "Not enough arguments.");
        return -1;
    }

    char *list_path = realpath(argv[1], NULL);
    if (!list_path) {

        fprintf(stderr, "Wrong path: %s\n", argv[1]);
        return -1;
    }

    int result = monitor(list_path);

    return result;
}
