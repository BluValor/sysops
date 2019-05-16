#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/times.h>
#include <memory.h>


char* TMP_DIR_NAME = "tmp_files";
char* REPORT_FILE_NAME = "wyniki.txt";

struct ExecResult {

    long double real_time;
    long double user_time;
    long double system_time;
    long double user_child_time;
    long double system_child_time;

    struct tms start_buffer;
    struct tms end_buffer;

    clock_t start_time;
    clock_t end_time;
};


struct ExecResult* allocate_def_exec_res() {

    return calloc(1, sizeof(struct ExecResult));
}


void set_start_time(struct ExecResult *exec_result) {

    exec_result -> start_time = times(& exec_result -> start_buffer);
}


void set_end_time(struct ExecResult *exec_result) {

    exec_result -> end_time = times(& exec_result -> end_buffer);
}


void calculate_times(struct ExecResult *exec_result) {

    exec_result -> real_time = (long double) (exec_result -> end_time - exec_result -> start_time) / sysconf(_SC_CLK_TCK);
    exec_result -> user_time = (long double) (exec_result -> end_buffer . tms_utime - exec_result -> start_buffer . tms_utime) / sysconf(_SC_CLK_TCK);
    exec_result -> system_time = (long double) (exec_result -> end_buffer . tms_stime - exec_result -> start_buffer . tms_stime) / sysconf(_SC_CLK_TCK);
    exec_result -> user_child_time = (long double) (exec_result -> end_buffer . tms_cutime - exec_result -> start_buffer . tms_cutime) / sysconf(_SC_CLK_TCK);
    exec_result -> system_child_time = (long double) (exec_result -> end_buffer . tms_cstime - exec_result -> start_buffer . tms_cstime) / sysconf(_SC_CLK_TCK);
}


void start_report(char* text) {

    FILE *report;
    report = fopen(REPORT_FILE_NAME, "aw");

    fprintf(report, "\n%s:\n%75s%10s\n\n", text, "utime", "stime");

    fclose(report);
}


void make_report(char* text, struct ExecResult *times) {

    FILE *report;
    report = fopen(REPORT_FILE_NAME, "aw");

    fprintf(report, "%-65s%10.2Lf%10.2Lf\n\n",
            text, times -> user_time, times -> system_time);

    fclose(report);
}


char gen_rand_char() {

    return rand() % (122 - 97 + 1) + 97;
}


void check_tmp_dir() {

    DIR *dir = opendir(TMP_DIR_NAME);

    if (dir)
        closedir(dir);
    else
        mkdir(TMP_DIR_NAME, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}


void delete_tmp_file(char* file_name) {

    char delete_str[64];
    snprintf(delete_str, sizeof delete_str, "rm -f ./%s/%s.txt", TMP_DIR_NAME, file_name);

    system(delete_str);
}


void generate(char* file_name, int amount, size_t length) {

    check_tmp_dir();

    delete_tmp_file(file_name);

    char open_str[64];
    snprintf(open_str, sizeof open_str, "./%s/%s.txt",TMP_DIR_NAME , file_name);

    FILE *file;
    file = fopen(open_str, "aw");

    for (int i = 0; i < amount; i++) {

        for (int j = 0; j < length; j++) {

            fprintf(file, "%c", gen_rand_char());
        }

        fprintf(file, "\n");
    }

    fclose(file);
}


/** for false arguments (too big values) returns NULL **/
char* sys_read_line(char *file_name, int line_nr, size_t line_length) {

    char read_file[64];
    snprintf(read_file, sizeof read_file, "./%s/%s.txt", TMP_DIR_NAME, file_name);

    int file;
    file = open(read_file, O_RDONLY);

    lseek(file, line_nr * (line_length + 1) * sizeof (char), SEEK_SET);

    char *ch = malloc(sizeof (char));
    read(file, ch, 1);
    if (*ch == 0) {

        free(ch);
        return NULL;
    }
    free(ch);

    lseek(file, line_nr * (line_length + 1) * sizeof (char), SEEK_SET);

    char *buffer = malloc((line_length + 1) * sizeof (char));
    read(file, buffer, line_length);
    buffer[line_length] = '\0';

    close(file);

    return buffer;
}


int sys_write_line(char *file_name, int line_nr, size_t line_length, char *line_buffer) {

    char write_file[64];
    snprintf(write_file, sizeof write_file, "./%s/%s.txt", TMP_DIR_NAME, file_name);

    int file;
    file = open(write_file, O_WRONLY);
    lseek(file, line_nr * (line_length + 1), SEEK_SET);

    write(file, line_buffer, line_length);
    write(file, "\n", 1);

    close(file);

    return 0;
}


/** for false arguments (too big values) returns NULL **/
char* lib_read_line(char *file_name, int line_nr, size_t line_length) {

    char read_file[64];
    snprintf(read_file, sizeof read_file, "./%s/%s.txt", TMP_DIR_NAME, file_name);

    FILE *file;
    file = fopen(read_file, "r");

    fseek(file, line_nr * (line_length + 1), 0);

    int ch;
    if ((ch = fgetc(file)) == EOF)
        return NULL;

    fseek(file, line_nr * (line_length + 1), 0);

    char *buffer = malloc((line_length + 1) * sizeof (char));
    fread(buffer, sizeof (char), line_length, file);
    buffer[line_length] = '\0';



    fclose(file);

    return buffer;
}


int lib_write_line(char *file_name, int line_nr, size_t line_length, char *line_buffer) {

    char write_file[64];
    snprintf(write_file, sizeof write_file, "./%s/%s.txt", TMP_DIR_NAME, file_name);

    FILE *file;
    file = fopen(write_file, "r+");
    fseek(file, line_nr * (line_length + 1), 0);

    fwrite(line_buffer, sizeof (char), line_length, file);
    fwrite("\n", sizeof (char), 1, file);

    fclose(file);

    return 0;
}


/** for false read arguments (too big values) returns -1 **/
int sys_sort(char *file_name, int lines, size_t line_length) {

    char *line1, *line2;

    for (int i = 0; i < lines - 1; i++) {

        int smallest = i;
        line1 = sys_read_line(file_name, i, line_length);

        if (line1 == NULL)
            return -1;

        for (int j = i + 1; j < lines; j++) {

            line2 = sys_read_line(file_name, j, line_length);

            if (line2 == NULL)
                return -1;

            if ((int) *line2 < (int) *line1) {

                smallest = j;
                free(line1);
                line1 = line2;
            } else
                free(line2);
        }

        line2 = sys_read_line(file_name, i, line_length);

        sys_write_line(file_name, i, line_length, line1);
        sys_write_line(file_name, smallest, line_length, line2);

        free(line1);
        free(line2);
    }

    return 0;
}


/** for false read arguments (too big values) returns -1 **/
int lib_sort(char *file_name, int lines, size_t line_length) {

    char *line1, *line2;

    for (int i = 0; i < lines - 1; i++) {

        int smallest = i;
        line1 = lib_read_line(file_name, i, line_length);

        if (line1 == NULL)
            return -1;

        for (int j = i + 1; j < lines; j++) {

            line2 = lib_read_line(file_name, j, line_length);

            if (line2 == NULL)
                return -1;

            if ((int) *line2 < (int) *line1) {

                smallest = j;
                free(line1);
                line1 = line2;
            } else
                free(line2);
        }

        line2 = lib_read_line(file_name, i, line_length);

        lib_write_line(file_name, i, line_length, line1);
        lib_write_line(file_name, smallest, line_length, line2);

        free(line1);
        free(line2);
    }
}


/** for false read arguments (too big values) returns -1 **/
int sys_copy(char *from_file, char *to_file, int lines, size_t line_length) {

    char write_to_f[64];
    snprintf(write_to_f, sizeof write_to_f, "./%s/%s.txt", TMP_DIR_NAME, to_file);

    int to_f;
    to_f = open(write_to_f, O_WRONLY);
    close(to_f);

    for (int i = 0; i < lines; i++) {

        char* line = sys_read_line(from_file, i, line_length);

        if (line == NULL)
            return -1;

        sys_write_line(to_file, i, line_length, line);

        free(line);
    }

    return 0;
}


/** for false read arguments (too big values) returns -1 **/
int lib_copy(char *from_file, char *to_file, int lines, size_t line_length) {

    char write_to_f[64];
    snprintf(write_to_f, sizeof write_to_f, "./%s/%s.txt", TMP_DIR_NAME, to_file);

    FILE *to_f;
    to_f = fopen(write_to_f, "w");
    fclose(to_f);

    for (int i = 0; i < lines; i++) {

        char* line = lib_read_line(from_file, i, line_length);

        if (line == NULL)
            return -1;

        lib_write_line(to_file, i, line_length, line);

        free(line);
    }

    return 0;
}


int duplicate_file(char *file_name, int lines, size_t line_length) {

    char copy_file[64];
    snprintf(copy_file, sizeof copy_file, "./%s/%s_copy.txt", TMP_DIR_NAME, file_name);

    char from_file[64];
    snprintf(from_file, sizeof from_file, "./%s/%s.txt", TMP_DIR_NAME, file_name);

    FILE *copy;
    copy = fopen(copy_file, "w");

    FILE *from_f;
    from_f = fopen(from_file, "r");

    char ch;

    while ((ch = fgetc(from_f)) != EOF)
        fputc(ch, copy);

    fclose(copy);
    fclose(from_f);

    return 0;
}


int main(int argc, char *argv[]) {

    char err_message[256];
    int err_reported = 0;

    for (int i = 1; i < argc; i++) {

        if (strncmp("generate", argv[i], 8) == 0) {

            if (i < argc - 3) {

                char* file_name = argv[i + 1];
                int lines = (int) strtol(argv[i + 2], NULL, 0);
                size_t line_length = (size_t) strtol(argv[i + 3], NULL, 0);

                generate(file_name, lines, line_length);

                i += 3;
                continue;
            } else {

                snprintf(err_message, sizeof err_message, "generate: not enough arguments");
                err_reported = 1;
                break;
            }
        }

        if (strncmp("sort", argv[i], 4) == 0) {

            if (i < argc - 4) {

                int check = 0;

                struct ExecResult *exec_result = allocate_def_exec_res();
                set_start_time(exec_result);

                char report_text[65];

                char* file_name = argv[i + 1];
                int lines = (int) strtol(argv[i + 2], NULL, 0);
                size_t line_length = (size_t) strtol(argv[i + 3], NULL, 0);

                if (strncmp("sys", argv[i + 4], 3) == 0) {

                    check = sys_sort(file_name, lines, line_length);
                    snprintf(report_text, sizeof report_text, "system sort - %s - %d x %d", file_name, lines, (int) line_length);

                } else if (strncmp("lib", argv[i + 4], 3) == 0) {

                    check = lib_sort(file_name, lines, line_length);
                    snprintf(report_text, sizeof report_text, "library sort - %s - %d x %d", file_name, lines, (int) line_length);

                } else {
                    
                    snprintf(err_message, sizeof err_message, "sort: unknown type (%s)", argv[i + 4]);
                    err_reported = 1;
                    free(exec_result);
                    break;
                }

                if (check == -1) {
                    
                    snprintf(err_message, sizeof err_message, "sort: too high values (%s, %d, %d)", file_name, lines, (int) line_length);
                    err_reported = 1;
                    free(exec_result);
                    break;
                }

                set_end_time(exec_result);
                calculate_times(exec_result);

                make_report(report_text, exec_result);

                i += 4;
                continue;
            } else {

                snprintf(err_message, sizeof err_message, "sort: not enough arguments");
                err_reported = 1;
                break;
            }
        }

        if (strncmp("copy", argv[i], 4) == 0) {

            if (i < argc - 4) {

                int check = 0;

                struct ExecResult *exec_result = allocate_def_exec_res();
                set_start_time(exec_result);

                char report_text[65];

                char* file_name_1 = argv[i + 1];
                char* file_name_2 = argv[i + 2];
                int lines = (int) strtol(argv[i + 3], NULL, 0);
                size_t line_length = (size_t) strtol(argv[i + 4], NULL, 0);

                if (strncmp("sys", argv[i + 5], 3) == 0) {

                    check = sys_copy(file_name_1, file_name_2, lines, line_length);
                    snprintf(report_text, sizeof report_text, "system copy - %s -> %s - %d x %d", file_name_1, file_name_2, lines, (int) line_length);

                } else if (strncmp("lib", argv[i + 5], 3) == 0) {

                    check = lib_copy(file_name_1, file_name_2, lines, line_length);
                    snprintf(report_text, sizeof report_text, "library copy - %s -> %s - %d x %d", file_name_1, file_name_2, lines, (int) line_length);

                } else {

                    snprintf(err_message, sizeof err_message, "copy: unknown type (%s)", argv[i + 5]);
                    err_reported = 1;
                    break;
                }

                if (check == -1) {

                    snprintf(err_message, sizeof err_message, "copy: too high values (%s, %d, %d)", file_name_1, lines, (int) line_length);
                    err_reported = 1;
                    free(exec_result);
                    break;
                }

                set_end_time(exec_result);
                calculate_times(exec_result);

                make_report(report_text, exec_result);

                i += 5;
                continue;
            } else {

                snprintf(err_message, sizeof err_message, "copy: not enough arguments");
                err_reported = 1;
                break;
            }
        }

        if (strncmp("duplicate", argv[i], 9) == 0) {

            if (i < argc - 3) {

                char* file_name = argv[i + 1];
                int lines = (int) strtol(argv[i + 2], NULL, 0);
                size_t line_length = (size_t) strtol(argv[i + 3], NULL, 0);

                duplicate_file(file_name, lines, line_length);

                i += 3;
                continue;
            } else {

                snprintf(err_message, sizeof err_message, "duplicate: not enough arguments");
                err_reported = 1;
                break;
            }
        }

        if (strncmp("report", argv[i], 6) == 0) {

            if (i < argc - 1) {

                char* text = argv[i + 1];

                start_report(text);

                i += 1;
                continue;
            } else {

                snprintf(err_message, sizeof err_message, "report: not enough arguments");
                err_reported = 1;
                break;
            }
        }
    }

    if (err_reported != 0)
        fprintf(stderr,"%s", err_message);
}
