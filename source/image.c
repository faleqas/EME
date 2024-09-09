#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



void load_image(struct Image* image,
                const char* path)
{
    assert(image != NULL);
    
    image->data = stbi_load(path,
                            &(image->w), &(image->h),
                            &(image->channels), 0);
}


void delete_image(struct Image* image)
{
    assert(image != NULL);
    free(image->data);
}



void _draw_image(struct Bitmap* bitmap,
                 byte* data,
                 int w, int h,
                 int channels,
                 int x, int y)
{
    if (x > bitmap->width) {
        return;
    }
    if (y > bitmap->height) {
        return;
    }
    if ((x + w) < 0) {
        return;
    }
    if ((y + h) < 0) {
        return;
    }
    
    int source_x = 0;
    int source_y = 0;
    
    size_t byte_count = w * h * channels;
    
    for (int i = 0; i < byte_count; i += channels)
    {
        byte red = data[i];
        byte green = data[i + 1];
        byte blue = data[i + 2];
        byte alpha = 255;
        if (channels == 4)
        {
            alpha = data[i + 3];
        }
        
        bitmap_draw_pixel(bitmap,
                          x + source_x, y + source_y,
                          red, green, blue, alpha);
        
        source_x++;
        if (source_x >= w) {
            source_x = 0;
            source_y++;
        }
    }
}


void resize_image(struct Image* image,
                   int new_w, int new_h)
{
    if ((image->resized_w == new_w) && (image->resized_h == new_h) && (image->resized_data))
    {
        return;
    }
    else if (image->resized_data)
    {
        free(image->resized_data);
    }

    int resized_byte_count = new_w * new_h * image->channels;
    
    image->resized_w = new_w;
    image->resized_h = new_h;
    image->resized_data = calloc(resized_byte_count, 1);
    
    float scale_x = (float)new_w / (float)image->w;
    float scale_y = (float)new_h / (float)image->h;
    
    {
        int dest_x = 0;
        int dest_y = 0;
        for (int i = 0; i < resized_byte_count; i += image->channels)
        {
            byte* r = image->resized_data + i;
            byte* g = image->resized_data + (i + 1);
            byte* b = image->resized_data + (i + 2);
            
            int source_x = (int)(floor(dest_x / (scale_x)));
            int source_y = (int)(floor(dest_y / (scale_y)));
            int source_index = source_x + (source_y * image->w);
            source_index *= image->channels;
            
            *r = *(image->data + source_index);
            *g = *(image->data + source_index + 1);
            *b = *(image->data + source_index + 2);
            
            dest_x++;
            if (dest_x >= new_w) {
                dest_x = 0;
                dest_y++;
            }
        }
    }
    
    return image->resized_data;
}


void draw_image(struct Bitmap* bitmap,
                const struct Image* image,
                int x, int y,
                int w, int h)
{
    byte* data;
    int true_w = w;
    int true_h = h;

    if ((w == 0) || (h == 0))
    {
        data = image->data;
        true_w = image->w;
        true_h = image->h;
    }
    else if ((image->w != w) || (image->h != h)) {
        resize_image(image, w, h);
        data = image->resized_data;
    }
    else {
        data = image->data;
    }
    
    _draw_image(bitmap, data, true_w, true_h, image->channels, x, y);
}