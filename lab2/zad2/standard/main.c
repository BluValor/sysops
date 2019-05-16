#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <ftw.h>
#include <limits.h>

char option;
struct tm *date;

const char * file_type(unsigned type) {

    switch( type ) {

        case DT_BLK: return "block dev";
        case DT_CHR: return "char dev";
        case DT_DIR: return "dir";
        case DT_FIFO: return "fifo";
        case DT_LNK: return "slink";
        case DT_REG: return "file";
        case DT_SOCK: return "sock";
        default: return "unknown type";
    };
}


struct tm* convert_time(char *time) {

    struct tm *result = malloc(sizeof (struct tm));
    strptime(time, "%F_%R", result);

    return result;
}


/** returns 0 if times are equal, negative if time2 is before time1, otherwise positive **/
int compare_times(struct tm *time1, struct tm *time2) {

    if (time1 -> tm_year == time2 -> tm_year && time1 -> tm_mon == time2 -> tm_mon && time1 -> tm_mday == time2 -> tm_mday
        && time1 -> tm_hour == time2 -> tm_hour && time1 -> tm_min == time2 -> tm_min)
        return 0;
    else if (mktime(time1) > mktime(time2))
        return -1;
    else return 1;
}


char* make_margin(int margin) {

    int part_size = 4 * margin;

    char* result = calloc(part_size, sizeof (char));

    for (int i = 0; i < part_size; i++) {

        result[i] = ' ';
    }

    return result;
}


void dir_tree_recur(char *path, int margin) {

    DIR* dir;

    if ((dir = opendir(path)) == NULL) {

        fprintf(stderr, "Cannot open given directory: %s", path);
    }

    struct dirent *f_info;
    char buffer[PATH_MAX + 1];

    chdir(path);

    int checker;
    long position;
    char* margin_arr = make_margin(margin);

    while ((f_info = readdir(dir)) != NULL) {

        if ((strlen(f_info -> d_name) == 1 && f_info -> d_name[0] == '.')
            || (strlen(f_info -> d_name) == 2 && f_info -> d_name[0] == '.' && f_info -> d_name[1] == '.'))
            continue;

        char *absolute_path = realpath(f_info -> d_name, buffer);

        struct stat *stats = malloc(sizeof (struct stat));

        lstat(absolute_path, stats);

        struct tm *access_time = localtime(& stats -> st_atime);
        struct tm *modify_time = localtime(& stats -> st_mtime);

        free(stats);

        checker = compare_times(date, modify_time);

        if ((option == '<' && checker >= 0) || (option == '>' && checker <= 0) || (option == '=' && checker != 0))
            continue;

        char atime[256], mtime[256];

        strftime(atime, 256, "%F_%R", access_time);
        strftime(mtime, 256, "%F_%R", modify_time);

        printf("\n%s%s\n%stype: %s | size: %ld bytes | last access: %s | last modify: %s\n", margin_arr, absolute_path,
               margin_arr, file_type(f_info -> d_type), (long int) stats -> st_size, atime, mtime);

        position = telldir(dir);

        if (f_info -> d_type == DT_DIR)
            dir_tree_recur(absolute_path, margin + 1);

        seekdir(dir, position);
    }

    closedir(dir);

    chdir("..");
}


int main(int argc, char *argv[]) {

    if (argc - 1 < 3) {

        fprintf(stderr, "Not enough arguments.");
        return -1;

    } else if (strlen(argv[3]) < 16) {

        fprintf(stderr, "Given date is too short.");
        return -1;
    }

    char *path = argv[1];
    option = *argv[2];
    date = convert_time(argv[3]);

    dir_tree_recur(path, 0);

    free(date);

    return 0;
}