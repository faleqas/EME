/* date = July 11th 2024 7:59 pm */

#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <stdint.h>
#include <stdbool.h>


#define TILEMAP_DEFAULT_W 24
#define TILEMAP_DEFAULT_H 12

//FREEING AND ALLOCATING THE GAME_MAP* ITSELF IS THE CALLER'S RESPONSIBILITY IF YOU WANT IT ON THE HEAP
struct Game_map
{
    int* tiles;
    
    int tilemap_w;
    int tilemap_h;
    int tile_count;
};


void game_map_create(struct Game_map* map, int tilemap_w, int tilemap_h);
void game_map_delete(struct Game_map* map);

void game_map_clear(struct Game_map* map);

bool game_map_save(const struct Game_map* map, const char* path);
bool game_map_load(struct Game_map* map, const char* path);

void game_map_set_tile(struct Game_map* map,
                       int tile_x, int tile_y, int new_id);

int game_map_get_tile_by_pos(const struct Game_map* map,
                             int tile_x, int tile_y);
int game_map_get_tile_by_index(const struct Game_map* map,
                               int index,
                               int* tile_x, int* tile_y);

#endif //GAME_MAP_H
