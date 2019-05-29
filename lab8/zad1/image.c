#include "image.h"
#include "shared_func.h"


void load_image(const char* path, image *buffer) {

    FILE* file = fopen(path, "r");
    if (file == NULL)
        die("Opening image failed!\n");

    char values[8192];
    int curr_line = -1;
    int index;

    while(fgets(values, 8192, file) != NULL) {

        if (values[0] == '#')
            continue;

        curr_line++;

        if (curr_line == 0 || curr_line == 2)
            continue;

        if (curr_line == 1) {

            char *v = strtok(values, " \r\n");

            if (v == NULL || (read_int_value(v, &buffer -> width)) == -1)
                die("Opening image failed! Bad description format.\n");

            v = strtok(NULL, " \r\n");

            if (v == NULL || (read_int_value(v, &buffer -> height)) == -1)
                die("Opening image failed! Bad description format.\n");

            new_image(buffer, buffer -> width, buffer -> height);

        } else {

            index = 0;
            char *v = strtok(values, " \r\n");

            while (v != NULL) {

                int value;
                if ((read_int_value(v, &value)) == -1 || value < 0)
                    die("Opening image failed! Bad format.\n");

                value = value > 255 ? 255 : value;

                buffer -> values[curr_line - 3][index++] = value;
                v = strtok(NULL, " \r\n");
            }

            if (index != buffer -> width)
                die("Opening image failed! Bad %d line width (%d / %d)!.\n", curr_line - 3, index, buffer -> width);
        }
    }

    if (curr_line - 2 != buffer -> height)
        die("Opening image failed! Bad total size (%d / %d).\n", curr_line - 2, buffer -> height);

    fclose(file);
}


void save_image(const char* path, image *buffer) {

    FILE* file = fopen(path, "w");
    if (file == NULL)
        die("Opening image output file failed!\n");

    fprintf(file, "P2 \r\n%d %d \r\n%d \r\n", buffer -> width, buffer -> height, MAX_IMG_VALUE);

    for (int h = 0; h < buffer -> height; h++) {

        for (int w = 0; w < buffer -> width; w++)
            fprintf(file, "%d ", buffer -> values[h][w]);

        fprintf(file, " \r\n");
    }

    fclose(file);
}


void new_image(image* buffer, int width, int height) {

    buffer -> width = width;
    buffer -> height = height;
    buffer -> values = calloc((size_t) height, sizeof(int*));

    for (int i = 0; i < height; i++)
        buffer -> values[i] = calloc((size_t) width, sizeof(int));
}


void delete_image(image* buffer) {

    buffer -> width = 0;
    buffer -> height = 0;

    for (int i = 0; i < buffer -> height; i++)
        free(buffer -> values[i]);

    free(buffer -> values);
}
