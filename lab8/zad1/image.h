#ifndef ZAD1_IMAGE_H
#define ZAD1_IMAGE_H


#define MAX_IMG_VALUE 255
#define MIN_IMG_VALUE 0


#include "exit_func.h"


typedef struct image {

    int** values;
    int height;
    int width;

} image;


void load_image(const char* path, image *buffer);
void save_image(const char* path, image *buffer);
void new_image(image* buffer, int width, int height);
void delete_image(image* buffer);


#endif //ZAD1_IMAGE_H
