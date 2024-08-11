/* date = July 14th 2024 3:53 pm */

#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "image.h"
#include "tile.h"

//TODO(omar): MAYBE THE ASSETMANAGER CAN BE A SINGLETON IN ITS .C FILE ?
//JUST LIKE THE EDITOR

struct AssetManager
{
    struct Image* images;
    int _images_count;
    
    struct Tile* tiles;
    int _tiles_count;
};



struct AssetManager* asset_manager_create();
void asset_manager_destroy(struct AssetManager* mng);


void asset_manager_load_image(struct AssetManager* mng,
                              const char* path, const int id);

void asset_manager_load_tile(struct AssetManager* mng,
                             const int id, const int image_id);

void asset_manager_load_tiles_from_file(struct AssetManager* mng, const char* path);

struct Image* 
asset_manager_get_image(const struct AssetManager* mng,
                        const int id);

struct Tile* 
asset_manager_get_tile(const struct AssetManager* mng,
                       const int id);


#endif //ASSET_MANAGER_H
