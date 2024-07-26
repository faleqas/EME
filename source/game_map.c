#include <assert.h>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include <stdbool.h>

#include "game_map.h"


void game_map_create(struct Game_map* map, int tilemap_w, int tilemap_h)
{
    if (tilemap_w <= 0) {
        tilemap_w = TILEMAP_DEFAULT_W;
    }
    if (tilemap_h <= 0) {
        tilemap_h = TILEMAP_DEFAULT_H;
    }
    
    if ((tilemap_w < 0) || (tilemap_h < 0)) {
        //this map will be loaded from game_map_load probably
        map->tiles = NULL;
        map->tile_count = 0;
        map->tilemap_w = 0;
        map->tilemap_h = 0;
        return;
    }
    
    map->tile_count = tilemap_w * tilemap_h;
    map->tiles = calloc(map->tile_count, sizeof(int));
    map->tilemap_w = tilemap_w;
    map->tilemap_h = tilemap_h;
}


void game_map_delete(struct Game_map* map)
{
    if (map) {
        if (map->tiles) {
            free(map->tiles);
        }
    }
}


void game_map_clear(struct Game_map* map)
{
    if (!(map->tiles)) {
        return;
    }
    
    for (int i = 0; i < map->tile_count; i++)
    {
        map->tiles[i] = 0;
    }
}


bool game_map_save(const struct Game_map* map, const char* path)
{
    FILE* fp;
    fopen_s(&fp, path, "wb");
    if (!fp) {
        return false;
    }
    
    _putw(map->tilemap_w, fp);
    _putw(map->tilemap_h, fp);
    
    for (int i = 0; i < map->tile_count; i++)
    {
        _putw(map->tiles[i], fp);
    }
    
    fclose(fp);
    
    return true;
}


bool game_map_load(struct Game_map* map, const char* path)
{
    FILE* fp;
    fopen_s(&fp, path, "rb");
    if (!fp) {
        return false;
    }
    
    if (map->tiles) {
        free(map->tiles);
    }
    
    int w = _getw(fp);
    int h = _getw(fp);
    int tile_count = map->tile_count;
    
    map->tile_count = tile_count;
    map->tiles = calloc(map->tile_count, sizeof(int));
    map->tilemap_w = w;
    map->tilemap_h = h;
    
    int id = -1;
    
    for (int i = 0; i < tile_count; i++)
    {
        id = _getw(fp);
        map->tiles[i] = id;
    }
    
    fclose(fp);
    
    return true;
}


void game_map_set_tile(struct Game_map* map,
                       int tile_x, int tile_y, int new_id)
{
    assert(map != NULL);
    
    int index = tile_x + (tile_y * map->tilemap_w);
    
    if (index >= (map->tilemap_w * map->tilemap_h)) {
        return;
    }
    if (index < 0) {
        return;
    }
    
    map->tiles[index] = new_id;
}


int game_map_get_tile_by_pos(const struct Game_map* map,
                             int tile_x, int tile_y)
{
    assert(map != NULL);
    
    int index = tile_x + (tile_y * map->tilemap_w);
    
    if (index >= (map->tilemap_w * map->tilemap_h)) {
        return -1;
    }
    if (index < 0) {
        return -1;
    }
    
    return map->tiles[index];
}


int game_map_get_tile_by_index(const struct Game_map* map,
                               int index,
                               int* tile_x, int* tile_y)
{
    if (index >= (map->tilemap_w * map->tilemap_h)) {
        return -1;
    }
    if (index < 0) {
        return -1;
    }
    
    if (tile_x) {
        *tile_x = index % map->tilemap_w;
    }
    if (tile_y) {
        *tile_y = index / map->tilemap_w;
    }
    
    return map->tiles[index];
}

