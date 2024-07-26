#include <assert.h>
#include <memory.h>
#include <malloc.h>

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
    
    for (int i = 0; i < mng->_images_count; i++)
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


struct Image* 
asset_manager_get_image(const struct AssetManager* mng,
                        const int id)
{
    if (id < 0) {
        return NULL;
    }
    
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
    
    for (int i = 0; i < mng->_tiles_count; i++)
    {
        if (mng->tiles[i].id == id) {
            return mng->tiles + i;
        }
    }
    
    return NULL;
}