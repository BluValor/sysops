#include "lib_finder.h"
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


// 1 >> file)name
// 2 >> /dev/null
// ./main search_directory katalog plik tmp 1>>results 2>>/dev/null

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


struct Commands {

};


char *REPORT_FILE_NAME = "raport2";


void create_table(int size) {

//    printf("\ncreate table %d", size);

    set_size(size);

    allocate_blocks();
}


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


void search_directory(char *dir, char *file, char *tmp_file_name) {

//    printf("\nsearch directory %s %s %s", dir, file, tmp_file_name);

    set_curr_dir(dir);
    set_curr_doc(file);

    execute_find(tmp_file_name);
}


void remove_block(int index) {

//    printf("\nremove block %d", index);

    free_block(index);
}


int insert_block(char *block) {

//    printf("\ninsert block");

    return add_block(block);
}


char* create_block(char* tmp_file_name) {

//    printf("\ncreate block %s", tmp_file_name);

    return get_find_result(tmp_file_name);
}


void make_report(char* tekst, struct ExecResult *times) {

    FILE *report;
    report = fopen(REPORT_FILE_NAME, "aw");

    fprintf(report, "%s:\nreal: %.2Lf   |   user (utime): %.2Lf   |   system (stime): %.2Lf   |   user (uctime): %.2Lf   |   system (sctime): %.2Lf\n\n",
            tekst, times -> real_time, times -> user_time, times -> system_time, times -> user_child_time, times -> system_child_time);

    fclose(report);
}


void add_and_delete_n_blocks(int n, char* tmp_file_name) {

    struct ExecResult *exec_result = allocate_def_exec_res();

    set_start_time(exec_result);

    for (int i = 0; i < n; i++) {

        char* block = get_find_result(tmp_file_name);
        int index = insert_block(block);
        remove_block(index);
    }

    set_end_time(exec_result);
    calculate_times(exec_result);

    char report_str[256];
    snprintf(report_str, sizeof report_str, "Added and removed %s file %d times", tmp_file_name, n);
    make_report(report_str, exec_result);
}


int main(int argc, char *argv[]) {

    int error_info = 0;
    char *endptr;

    char *recent_block = NULL;
    char *recent_block_file_name = NULL;
    struct ExecResult *recent_block_times = NULL;

    for (int i = 0; i < argc; i++) {

        if (strncmp("create_table", argv[i], 12) == 0 && get_size() == -1) {

            if (i < argc - 1) {

                i++;
                create_table((int) strtol(argv[i], &endptr, 0));
                continue;
            }
            else {

                error_info = -1;
                break;
            }
        }

        if (strncmp("search_directory", argv[i], 16) == 0) {

            if (i < argc - 3) {

                struct ExecResult *exec_result = allocate_def_exec_res();
                set_start_time(exec_result);

                search_directory(argv[i + 1], argv[i + 2], argv[i + 3]);

                set_end_time(exec_result);
                calculate_times(exec_result);

                char report_str[256];
                snprintf(report_str, sizeof report_str, "search_directory (directory: %s, file: %s, temporary file name: %s)",
                        argv[i + 1], argv[i + 2], argv[i + 3]);
                make_report(report_str, exec_result);

                free(exec_result);

                i += 3;
                continue;
            }
            else {

                error_info = -1;
                break;
            }
        }

        if (strncmp("insert_block", argv[i], 12) == 0 && get_size() != -1) {

            if (recent_block != NULL) {

                struct ExecResult *exec_result = allocate_def_exec_res();
                set_start_time(exec_result);

                int position = insert_block(recent_block);

                set_end_time(exec_result);
                calculate_times(exec_result);

                if (recent_block_times != NULL) {

                    struct ExecResult *full_exec_result = allocate_def_exec_res();
                    full_exec_result -> real_time = exec_result -> real_time + recent_block_times -> real_time;
                    full_exec_result -> user_time = exec_result -> user_time + recent_block_times -> user_time;
                    full_exec_result -> system_time = exec_result -> system_time + recent_block_times -> system_time;
                    full_exec_result -> user_child_time = exec_result -> user_child_time + recent_block_times -> user_child_time;
                    full_exec_result -> system_child_time = exec_result -> system_child_time + recent_block_times -> system_child_time;

                    char report_str[256];
                    snprintf(report_str, sizeof report_str, "create_block + insert_block: %s to position %d",
                            recent_block_file_name, position);
                    make_report(report_str, full_exec_result);

                    free(full_exec_result);
                    free(recent_block_times);
                    recent_block_times = NULL;
                } else {

                    char report_str[256];
                    snprintf(report_str, sizeof report_str, "insert_block: ");
                    make_report(report_str, exec_result);
                }

                free(exec_result);
                continue;
            }
            else {

                error_info = -1;
                break;
            }
        }

        if (strncmp("create_block", argv[i], 12) == 0) {

            if (i < argc - 1) {

                i++;

                if (recent_block_times != NULL)
                    free(recent_block_times);

                struct ExecResult *exec_result = allocate_def_exec_res();
                set_start_time(exec_result);

                recent_block = create_block(argv[i]);

                set_end_time(exec_result);
                calculate_times(exec_result);

                recent_block_times = exec_result;
                recent_block_file_name = argv[i];

                continue;
            }
            else {

                error_info = -1;
                break;
            }
        }

        if (strncmp("remove_block", argv[i], 12) == 0 && get_size() != -1) {

            if (i < argc - 1) {

                i++;

                struct ExecResult *exec_result = allocate_def_exec_res();
                set_start_time(exec_result);

                remove_block((int) strtol(argv[i], &endptr, 0));

                set_end_time(exec_result);
                calculate_times(exec_result);

                char report_str[256];
                snprintf(report_str, sizeof report_str, "remove_block: %s", argv[i]);
                make_report(report_str, exec_result);

                free(exec_result);
                continue;
            }
            else {

                error_info = -1;
                break;
            }
        }

        if (strncmp("add_and_delete_n_blocks", argv[i], 12) == 0 && get_size() != -1) {

            if (i < argc - 2) {

                add_and_delete_n_blocks((int) strtol(argv[i + 1], &endptr, 0), argv[i + 2]);
                i += 2;
                continue;
            }
            else {

                error_info = -1;
                break;
            }
        }

    }

    clean_up();

    return 0;
}



