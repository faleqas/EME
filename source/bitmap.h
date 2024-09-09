/* date = July 8th 2024 4:22 pm */

#ifndef BITMAP_H
#define BITMAP_H

#include "defines.h"

//general purpose bitmap structure
//allows for expanding outside win32 bitmaps (multiplatform EME ?)
struct Bitmap
{
    int width;
    int height;
    
    int bytes_per_pixel;
    
    byte* data;
};


//ALL THESE FUNCTIONS ONLY WORK FOR bytes_per_pixel = 4
void bitmap_clear(struct Bitmap* bitmap, byte r, byte g, byte b);

void bitmap_draw_pixel(struct Bitmap* bitmap,
                       int x, int y,
                       byte r, byte g, byte b, byte a);

void bitmap_draw_rect(struct Bitmap* bitmap,
                      int x, int y,
                      int w, int h,
                      byte r, byte g, byte b, byte a);


void bitmap_draw_rect_outline(struct Bitmap* bitmap,
                              int x, int y,
                              int w, int h,
                              byte r, byte g, byte b,
                              int thickness);

#endif //BITMAP_H
