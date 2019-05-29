#include "filter.h"
#include "shared_func.h"


void load_filter(const char* path, filter *buffer) {

    FILE* file = fopen(path, "r");
    if (file == NULL)
        die("Opening filter failed!\n");

    char values[4096];
    int curr_line = -1;
    int index;

    while(fgets(values, 4096, file) != NULL) {

        curr_line++;

        if (curr_line == 0) {

            char *v = strtok(values, " \r\n");

            if (v == NULL || (read_int_value(v, &buffer -> size)) == -1)
                die("Opening filter failed! Bad description format.\n");

            new_filter(buffer, buffer->size);

        } else if (curr_line == buffer -> size + 1) {

            char *v = strtok(values, " \r\n");

            if (v == NULL || (read_int_value(v, &buffer -> divider)) == -1)
                die("Opening filter failed! Bad divider format.\n");

        } else {

            index = 0;
            char *v = strtok(values, " \r\n");

            while (v != NULL) {

                int value;
                if ((read_int_value(v, &value)) == -1)
                    die("Opening filter failed! Bad format.\n");

                buffer -> v_ints[curr_line - 1][index++] = value;
                v = strtok(NULL, " \r\n");
            }

            if (index != buffer -> size)
                die("Opening image failed! Bad %d line width (%d / %d)!.\n", curr_line - 1, index, buffer -> size);
        }
    }

    if (curr_line > buffer -> size + 1)
        die("Opening filter failed! Bad total size (%d / %d).\n", curr_line, buffer -> size);

    prepare_filter(buffer);

    fclose(file);
}


void save_filter(const char* path, filter *buffer) {

    FILE* file = fopen(path, "w");
    if (file == NULL)
        die("Opening filter output file failed!\n");

    fprintf(file, "%d \r\n", buffer -> size);

    for (int h = 0; h < buffer -> size; h++) {

        for (int w = 0; w < buffer -> size; w++)
            fprintf(file, "%d ", buffer -> v_ints[h][w]);

        fprintf(file, " \r\n");
    }

    fprintf(file, "%d", buffer -> divider);

    fclose(file);
}


void new_filter(filter* buffer, int size) {

    buffer -> size = size;
    buffer -> divider = 1;

    buffer -> v_ints = calloc((size_t) size, sizeof(int*));
    for (int i = 0; i < size; i++)
        buffer -> v_ints[i] = calloc((size_t) size, sizeof(int));

    buffer -> v_floats = calloc((size_t) size, sizeof(float*));
    for (int i = 0; i < size; i++)
        buffer -> v_floats[i] = calloc((size_t) size, sizeof(float));
}


void delete_filter(filter* buffer) {

    buffer -> size = 0;

    for (int i = 0; i < buffer -> size; i++)
        free(buffer -> v_ints[i]);
    free(buffer -> v_ints);

    for (int i = 0; i < buffer -> size; i++)
        free(buffer -> v_floats[i]);
    free(buffer -> v_floats);
}


void prepare_filter(filter *buffer) {

    for (int y = 0; y < buffer -> size; y++)
        for (int x  = 0; x < buffer -> size; x++)
            buffer -> v_floats[y][x] = (float) buffer -> v_ints[y][x] / buffer -> divider;
}


//void prepare_filter(filter *buffer) {
//
//    float pos_sum = 0.0;
//    float neg_sum = 0.0;
//
//    for (int y = 0; y < buffer -> size; y++)
//        for (int x  = 0; x < buffer -> size; x++)
//            if (buffer -> v_ints[y][x] > 0)
//                pos_sum += buffer -> v_ints[y][x];
//            else
//                neg_sum -= buffer -> v_ints[y][x];
//
//    if (pos_sum == 0.0 && neg_sum > 0.0)
//        die("Invalid filter!\n");
//
//    if (pos_sum > 0 && neg_sum > 0) {
//
//        for (int y = 0; y < buffer->size; y++)
//            for (int x = 0; x < buffer->size; x++)
//                if (buffer->v_ints[y][x] > 0)
//                    buffer->v_floats[y][x] = 2 * buffer->v_ints[y][x] / pos_sum;
//                else
//                    buffer->v_floats[y][x] = buffer->v_ints[y][x] / neg_sum;
//
//    } else {
//
//        for (int y = 0; y < buffer->size; y++)
//            for (int x = 0; x < buffer->size; x++)
//                buffer->v_floats[y][x] = buffer->v_ints[y][x] / pos_sum;
//    }
//
//}
