#define _XOPEN_SOURCE 1
#define _XOPEN_SOURCE_EXTENDED 1
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


int display_info(const char *path, const struct stat *sb, int flag, struct FTW *ftwbuf) {

    char *margin = make_margin(ftwbuf -> level);

    struct tm *access_time = localtime(& sb -> st_atime);
    struct tm *modify_time = localtime(& sb -> st_mtime);

    int checker = compare_times(date, modify_time);

    if ((option == '<' && checker >= 0) || (option == '>' && checker <= 0) || (option == '=' && checker != 0))
        return 0;

    char atime[256], mtime[256];

    strftime(atime, 256, "%F_%R", access_time);
    strftime(mtime, 256, "%F_%R", modify_time);

    char r_path[4097];
    realpath(path, r_path);

    printf("\n%s%s\n%stype: %s | size: %ld bytes | last access: %s | last modify: %s\n", margin, r_path, margin,
           (flag == FTW_D) ? "dir" : (flag == FTW_F) ? "file" : (flag == FTW_DNR) ? "dnr" :
           (flag == FTW_SL) ? "slink" : (flag == FTW_NS) ? "ns" : "unknown", sb ->st_size, atime, mtime);

    free(margin);
    return 0;
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

    int result = nftw(path, display_info, 9000, FTW_PHYS);

    if (result != 0)
        fprintf(stderr, "error occured (probably: bad input path value)");

    free(date);

    return 0;
}