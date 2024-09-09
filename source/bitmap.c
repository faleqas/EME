#include <stdint.h>

#include "bitmap.h"


//ALPHA BLENDING IS BODGED TO ONLY BLEND WITH THE BG COLOR. DRAWING MULTIPLE SHIT ON TOP OF EACH OTHER WON'T DO CORRECT BLENDING
//TODO(omar): fix that


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
                
                *pixel = 0xff; //alpha
                pixel++;
            }
            row += pitch;
        }
    }
}


void bitmap_draw_pixel(struct Bitmap* bitmap,
                       int x, int y,
                       byte r, byte g, byte b, byte a)
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

    // r = (r * a) / 255;
    // g = (g * a) / 255;
    // b = (b * a) / 255;
    
    if (bitmap->data && (bitmap->bytes_per_pixel == 4)) {
        uint32_t* pixel = (uint32_t*)(bitmap->data);
        int pixel_index = x + (y * bitmap->width);
        pixel += pixel_index;

        
        
        uint8_t* blue_address = (uint8_t*)pixel;
        uint8_t* green_address = (uint8_t*)pixel + 1;
        uint8_t* red_address = (uint8_t*)pixel + 2;
        uint8_t* alpha_address = (uint8_t*)pixel + 3;

        uint8_t src_blue = *blue_address;
        uint8_t src_green = *green_address;
        uint8_t src_red = *red_address;
        uint8_t src_alpha = *alpha_address;

        float falpha = (float)(a) / 255.0f;

        *blue_address = falpha * b + (1 - falpha) * 70;
        *green_address = falpha * g + (1 - falpha) * 50;
        *red_address = falpha * r + (1 - falpha) * 50;
    }
}


void bitmap_draw_rect(struct Bitmap* bitmap,
                      int x, int y,
                      int w, int h,
                      byte r, byte g, byte b, byte a)
{
    for (int pixel_x = x; pixel_x < x + w; pixel_x++)
    {
        for (int pixel_y = y; pixel_y < y + h; pixel_y++)
        {
            bitmap_draw_pixel(bitmap, pixel_x, pixel_y,
                              r, g, b, a);
        }
    }
}

void bitmap_draw_rect_outline(struct Bitmap* bitmap,
                              int x, int y,
                              int w, int h,
                              byte r, byte g, byte b, byte a,
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
    
    bitmap_draw_rect(bitmap, x, y, w, thickness, r, g, b, a); //top
    
    bitmap_draw_rect(bitmap, x, y + h, w, thickness, r, g, b, a); //down
    
    bitmap_draw_rect(bitmap, x, y, thickness, h, r, g, b, a); //left
    
    bitmap_draw_rect(bitmap, x + w, y, thickness, h + thickness, r, g, b, a); //right
}