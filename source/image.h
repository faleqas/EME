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


struct Image
{
    int id; //if you loaded the image yourself and not through
    //through the asset manager this is not needed
    int w;
    int h;
    int channels;
    byte* data;
};


void load_image(struct Image* image,
                const char* path);


void _draw_image(struct Bitmap* bitmap,
                 byte* data,
                 int w, int h,
                 int channels,
                 int x, int y);

byte* resize_image(const struct Image* image,
                   int new_w, int new_h);

void draw_image(struct Bitmap* bitmap,
                const struct Image* image,
                int x, int y,
                int w, int h);

#endif //IMAGE_H
