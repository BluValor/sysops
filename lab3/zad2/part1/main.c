#define TIME_PATTERN "_%F_%H-%M-%S_"
#define ARCHIVE_PATH "./archive/"

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


int memory_cpy(char *file_path, char *file_name, int interval, int monitor_time) {

    int modified_files = 0;

    size_t file_size = (size_t) get_file_size(file_path);
    char *file_content = get_file_content(file_path);

    if (!file_content)
        exit(modified_files);

    struct stat stats;
    stat(file_path, &stats);
    time_t prev_mod = stats . st_mtime;

    for (int i = 0; i < monitor_time / interval; i++) {

        sleep((unsigned int) interval);

        stat(file_path, &stats);

        if (prev_mod == stats . st_mtime)
            continue;

        prev_mod = stats . st_mtime;

        if (make_copy(file_path, file_name, file_content, file_size, prev_mod) < 0) {

            free(file_content);
            exit(modified_files);
        }

        free(file_content);
        file_size = (size_t) get_file_size(file_path);
        file_content = get_file_content(file_path);

        if (!file_content)
            exit(modified_files);

        modified_files++;
    }

    free(file_content);
    return modified_files;
}


int exec_cpy(char *file_path, char *file_name, int interval, int monitor_time) {

    int modified_files = 0;

    struct stat stats;
    stat(file_path, &stats);
    time_t prev_mod = stats . st_mtime;

    char *real_archive_path = realpath(ARCHIVE_PATH, NULL);

    if (!real_archive_path) {

        fprintf(stderr, "Wrong path: %s", real_archive_path);
        exit(modified_files);
    }

    char *copy_path = malloc(256 * sizeof (char));
    sprintf(copy_path, "%s/%s%s.txt", real_archive_path, file_name, format_time(prev_mod));

    int exec_cpy_pid = fork();

    if (exec_cpy_pid == 0) {

        execl("/bin/cp", "cp", file_path, copy_path, NULL);
        exit(0);

    } else {

        int c_exit_status;
        waitpid(exec_cpy_pid, &c_exit_status, 0);

        if (c_exit_status != 0) {

            free(copy_path);
            fprintf(stderr, "Copying failed for file: %s", file_path);
            exit(modified_files);
        }
    }

    modified_files++;

    for (int i = 0; i < monitor_time / interval; i++) {

        sleep((unsigned int) interval);

        stat(file_path, &stats);

        if (prev_mod == stats . st_mtime)
            continue;

        prev_mod = stats . st_mtime;

        exec_cpy_pid = fork();

        if (exec_cpy_pid == 0) {

            copy_path = malloc(256 * sizeof(char));
            sprintf(copy_path, "%s%s%s.txt", ARCHIVE_PATH, file_name, format_time(prev_mod));

            execl("/bin/cp", "cp", file_path, copy_path, (char *) 0);

            exit(0);

        } else {

            int c_exit_status;
            waitpid(exec_cpy_pid, &c_exit_status, 0);

            if (c_exit_status != 0) {

                free(copy_path);
                fprintf(stderr, "Copying failed for file: %s", file_path);
                exit(modified_files);
            }
        }

        modified_files++;

    }

    free(copy_path);

    return modified_files;
}


int monitor(char *list_path, int monitor_time, char *option) {

    int lines_nr;
    if ((lines_nr = count_lines(list_path)) == -1)
        return -1;

    pid_t *proc_pids = malloc(lines_nr * sizeof(pid_t));

    for (int i = 0; i < lines_nr; i++) {

        char *line = read_line(list_path, i);

        char *file_name = read_till_delim(line, 1, ' ');
        char *file_path = realpath(read_till_delim(line, 2, ' '), NULL);

        if (!list_path) {

            fprintf(stderr, "Wrong path: %s", read_till_delim(line, 2, ' '));
            return -1;
        }

        char *tmp = read_till_delim(line, 3, ' ');
        int interval = (int) strtol(tmp, NULL, 0);

        free(tmp);
        if (interval < 0) {

            fprintf(stderr, "Wrong time interval: %d, for file: %s.", interval, file_name);
            free(file_name);
            free(file_path);
            return -1;
        }
        free(line);

        int child_pid = fork();
        int modified_files = 0;

        if (child_pid == 0) {

            if (strcmp(option, "memory_cpy") == 0) {

                modified_files = memory_cpy(file_path, file_name, interval, monitor_time);

            } else {

                modified_files = exec_cpy(file_path, file_name, interval, monitor_time);
            }

            _exit(modified_files);

        } else {

            proc_pids[i] = child_pid;
        }

        free(file_name);
        free(file_path);
    }

    for (int i = 0; i < lines_nr; i++) {

        int c_exit_status;
        waitpid(proc_pids[i], &c_exit_status, 0);
        printf("Process %d made %d copies of file.\n", proc_pids[i], WEXITSTATUS(c_exit_status));
    }

    free(proc_pids);
    return 0;
}


int main(int argc, char *argv[]) {

    if (argc - 1 < 3) {

        fprintf(stderr, "Not enough arguments.");
        return -1;
    }

    char *list_path = realpath(argv[1], NULL);
    if (!list_path) {

        fprintf(stderr, "Wrong path: %s", argv[1]);
        return -1;
    }

    int monitor_time = (int) strtol(argv[2], NULL, 0);
    if (monitor_time < 0) {

        fprintf(stderr, "Wrong time: %d", monitor_time);
        return -1;
    }

    char *option = argv[3];
    if (strcmp(option, "memory_cpy") != 0 && strcmp(option, "exec_cpy") != 0) {

        fprintf(stderr, "Wrong mode: %s", option);
        return -1;
    }

    int result = monitor(list_path, monitor_time, option);

    return result;
}
