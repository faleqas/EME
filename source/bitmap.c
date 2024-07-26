#include <stdint.h>

#include "bitmap.h"


void bitmap_clear(struct Bitmap* bitmap, byte r, byte g, byte b)
{
    if (bitmap->data && (bitmap->bytes_per_pixel == 4)) {
        byte* row = bitmap->data;
        int pitch = bitmap->width * bitmap->bytes_per_pixel;
        
        for (int y = 0; y < bitmap->height; y++) {
            byte* pixel = row;
            for (int x = 0; x < bitmap->width; x++) {
                *pixel = b; //blue
                pixel++;
                
                *pixel = g; //green
                pixel++;
                
                *pixel = r; //red
                pixel++;
                
                //*pixel = 0; //pad
                pixel++;
            }
            row += pitch;
        }
    }
}


void bitmap_draw_pixel(struct Bitmap* bitmap,
                       int x, int y,
                       byte r, byte g, byte b)
{
    if (x < 0) {
        return;
    }
    else if (y < 0) {
        return;
    }
    else if (x >= bitmap->width) {
        return;
    }
    else if (y >= bitmap->height) {
        return;
    }
    
    if (bitmap->data && (bitmap->bytes_per_pixel == 4)) {
        uint32_t* pixel = (uint32_t*)(bitmap->data);
        int pixel_index = x + (y * bitmap->width);
        pixel += pixel_index;
        
        uint8_t* color = (uint8_t*)pixel;
        *color = b; //blue
        color++;
        
        *color = g; //green
        color++;
        
        *color = r; //red
        color++;
    }
}


void bitmap_draw_rect(struct Bitmap* bitmap,
                      int x, int y,
                      int w, int h,
                      byte r, byte g, byte b)
{
    for (int pixel_x = x; pixel_x < x + w; pixel_x++)
    {
        for (int pixel_y = y; pixel_y < y + h; pixel_y++)
        {
            bitmap_draw_pixel(bitmap, pixel_x, pixel_y,
                              r, g, b);
        }
    }
}

void bitmap_draw_rect_outline(struct Bitmap* bitmap,
                              int x, int y,
                              int w, int h,
                              byte r, byte g, byte b,
                              int thickness)
{
    /*
    if (x < 0) {
        return;
    }
    if (y < 0) {
        return;
    }
    if ((x + w) > bitmap->width) {
        return;
    }
    if ((y + h) > bitmap->height) {
        return;
    }*/
    
    bitmap_draw_rect(bitmap, x, y, w, thickness, r, g, b); //top
    
    bitmap_draw_rect(bitmap, x, y + h, w, thickness, r, g, b); //down
    
    bitmap_draw_rect(bitmap, x, y, thickness, h, r, g, b); //left
    
    bitmap_draw_rect(bitmap, x + w, y, thickness, h + thickness, r, g, b); //right
}