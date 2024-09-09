/* date = July 14th 2024 3:25 pm */

#ifndef IMAGE_H
#define IMAGE_H


#include "bitmap.h"
#include "defines.h"


enum
{
    IMAGE_NONE,
    IMAGE_TEST
};

//each Image only has 1 resized version that you can change by calling resize_image().
//after using resize_image you can choose to draw the original image or the current resized version using draw_image()
struct Image
{
    int id; //if you loaded the image yourself and not through
    //through the asset manager this is not needed
    int w;
    int h;
    int channels;
    byte* data;

    int resized_w;
    int resized_h;
    byte* resized_data;
};


void load_image(struct Image* image,
                const char* path);


void _draw_image(struct Bitmap* bitmap,
                 byte* data,
                 int w, int h,
                 int channels,
                 int x, int y);

void resize_image(struct Image* image,
                   int new_w, int new_h);

void draw_image(struct Bitmap* bitmap,
                const struct Image* image,
                int x, int y,
                int w, int h);

#endif //IMAGE_H
