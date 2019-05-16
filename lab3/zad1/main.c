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


int display_info(const char *path, const struct stat *sb, int flag, struct FTW *ftwbuf) {

    if (flag == FTW_D) {

        printf("\n%s\n", path);

        pid_t child_pid = fork();

        if (child_pid != 0) {

            wait(NULL);

        } else {
            printf("Child PID: %d", (int) getpid());
            printf("\n");
            execl("/bin/ls", "ls", "-l", path, NULL);
        }
    }

    return 0;
}


int main(int argc, char *argv[]) {

    if (argc - 1 < 1) {

        fprintf(stderr, "Not enough arguments.");
        return -1;
    }

    char *path = argv[1];

    int result = nftw(path, display_info, 9000, FTW_PHYS);

    if (result != 0)
        fprintf(stderr, "error occured (probably: bad input path value)");

    return 0;
}