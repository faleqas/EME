#include <assert.h>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asset_manager.h"


#define INITIAL_IMAGE_COUNT 32
#define INITIAL_TILE_COUNT 32


struct AssetManager* asset_manager_create()
{
    struct AssetManager* mng = malloc(sizeof(struct AssetManager));
    
    mng->images = calloc(INITIAL_IMAGE_COUNT,
                         sizeof(struct Image));
    mng->_images_count = INITIAL_IMAGE_COUNT;
    
    mng->tiles = calloc(INITIAL_TILE_COUNT,
                        sizeof(struct Tile));
    mng->_tiles_count = INITIAL_TILE_COUNT;
    
    return mng;
}


void asset_manager_destroy(struct AssetManager* mng)
{
    assert(mng != NULL);
    
    free(mng->images);
    free(mng->tiles);
    free(mng);
}


void asset_manager_load_image(struct AssetManager* mng,
                              const char* path, int id)
{
    if (id < 0) {
        return;
    }
    
    for (int i = 0; i < mng->_images_count; i++)
    {
        if (mng->images[i].id == 0) {
            struct Image* img = mng->images + i;
            load_image(img, path);
            if (img->data) {
                img->id = id;
            }
            else {
                //loading image failed. path is probably null
                //we MIGHT have to tell the user :)
                img->id = 0;
            }
            return;
        }
    }
    
    //not enough space. reallocate
    
    //TODO(omar): move this to a function
    int prev_size = mng->_images_count;
    mng->_images_count += INITIAL_IMAGE_COUNT;
    mng->images = realloc(mng->images,
                          sizeof(struct Image) * mng->_images_count);
    
    //clearing
    for (int i = prev_size; i < mng->_images_count;
         i++)
    {
        mng->images[i].id = 0;
    }
}


void asset_manager_load_tile(struct AssetManager* mng,
                             const int id, const int image_id)
{
    if (id < 0) {
        return;
    }
    
    for (int i = 0; i < mng->_tiles_count; i++)
    {
        if (mng->tiles[i].id == 0) {
            mng->tiles[i].id = id;
            mng->tiles[i].image_id = image_id;
            return;
        }
    }
    
    //not enough space. reallocate
    
    //TODO(omar): move this to a function
    int prev_size = mng->_tiles_count;
    mng->_tiles_count += INITIAL_TILE_COUNT;
    mng->tiles = realloc(mng->tiles,
                         sizeof(struct Tile) * mng->_tiles_count);
    
    //clearing
    for (int i = prev_size; i < mng->_tiles_count;
         i++)
    {
        mng->tiles[i].id = 0;
    }
}



static char* trim_whitespace(const char* str)
{
    int len = strlen(str);
    char* result = calloc(len+1, sizeof(char));

    int i = 0;
    for (int j = 0; j < len; j++)
    {
        if ((str[j] != ' ') && (str[j] != '\n')) {
            result[i] = str[j];
            i++;
        }
    }

    return result;
}


static void _split_string_push_to_result(char*** result, int* result_len, char* str)
{
    (*result_len)++;
    if (!(*result))
    {
        *result = malloc(sizeof(char*) * (*result_len));
    }
    else
    {
        *result = realloc(*result, sizeof(char*) * (*result_len));
    }

    (*result)[(*result_len) - 1] = str;
}


static char** split_string(const char* str, const char delimiter, int* out_len)
{
    if (!out_len)
    {
        return NULL;
    }

    char** result = NULL;
    int result_len = 0;

    char* current_str = NULL;
    int current_str_len = 1; //counting the null terminator
    for (int i = 0; i < strlen(str); i++)
    {
        if ((str[i] == delimiter))
        {
            _split_string_push_to_result(&result, &result_len, current_str);
            current_str = NULL;
            current_str_len = 1;
            continue;
        }

        current_str_len++;
        if (!current_str)
        {
            current_str = malloc(sizeof(char) * current_str_len);
        }
        else
        {
            current_str = realloc(current_str, sizeof(char) * current_str_len);
        }

        current_str[current_str_len - 2] = str[i];
        current_str[current_str_len - 1] = 0;

        if (i == (strlen(str) - 1))
        {
            if (current_str)
            {
                _split_string_push_to_result(&result, &result_len, current_str);
                current_str = NULL;
                current_str_len = 1;
            }
        }
    }

    *out_len = result_len;
    return result;
}


static void split_string_free_result(char** result, int result_len)
{
    if (!result) return;

    for (int i = 0; i < result_len; i++)
    {
        if (result[i])
        {
            free(result[i]);
        }
    }

    free(result);
}


void asset_manager_load_tiles_from_file(struct AssetManager* mng, const char* path)
{
    FILE* fp;
    fopen_s(&fp, path, "r");
    
    if (!fp) return;
    
    char str[256];
    while (!feof(fp))
    {
        fgets(str, 256, fp);
        char* trimmed_str = trim_whitespace(str);

        int parameters_len = 0;
        char** parameters = split_string(trimmed_str, ':', &parameters_len);

        if (parameters_len == 3)
        {
            const char* tile_name = parameters[0];
            int tile_id = atoi(parameters[1]);
            const char* tile_sprite_path = parameters[2];

            asset_manager_load_image(mng,
                tile_sprite_path, tile_id);

            asset_manager_load_tile(mng,
                tile_id, tile_id);
        }

        split_string_free_result(parameters, parameters_len);
        free(trimmed_str);
    }
    
    fclose(fp);
}


struct Image* 
asset_manager_get_image(const struct AssetManager* mng,
                        const int id)
{
    if (id < 0) {
        return NULL;
    }

    //TODO(omar): make sure this optimization will always work
    return (mng->images + (id - 1));
    
    for (int i = 0; i < mng->_images_count; i++)
    {
        if (mng->images[i].id == id) {
            return mng->images + i;
        }
    }
    
    return NULL;
}



struct Tile* 
asset_manager_get_tile(const struct AssetManager* mng,
                       const int id)
{
    if (id < 0) {
        return NULL;
    }

    //TODO(omar): make sure this optimization will always work
    return (mng->tiles + (id - 1));
    
    for (int i = 0; i < mng->_tiles_count; i++)
    {
        if (mng->tiles[i].id == id) {
            return mng->tiles + i;
        }
    }
    
    return NULL;
}


struct Tile*
asset_manager_get_tile_from_index(const struct AssetManager* mng, const int index)
{
    if (index < 0)
    {
        return NULL;
    }

    if (index >= (mng->_tiles_count))
    {
        return NULL;
    }

    struct Tile* tile = mng->tiles + index;
    if (tile->id == 0)
    {
        return NULL;
    }

    return tile;
}