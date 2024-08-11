#ifndef EME__EDITOR
#define EME__EDITOR

#include <stdint.h>
#include <stdbool.h>

#include "defines.h"
#include "bitmap.h"
#include "game_map.h"
#include "input.h"

#include "asset_manager.h"
#include "camera.h"
#include "cursor.h"


//Menu Items
enum
{
    MI_NONE,
    MI_LOADMAP,
    MI_SAVEMAP,
    MI_CLEARMAP
};


//EDITOR STATES
enum
{
    E_PLAY_AREA,
    E_SELECT_TILE
};


struct Editor
{
    int state;
    int client_w;
    int client_h;

    int selected_tile_id;
    
    struct AssetManager* asset_mng;
    
    struct mouse_state mouse;
    struct keyboard_state keyboard;
    
    //bitmap pixel data is allocated and freed by the platform layer
    //since calling the specific allocation functions for the OS
    //is faster than malloc()
    //(as if we care about speed)
    struct Bitmap bitmap;
    
    struct Cursor cursor;
    struct Game_map game_map;
    int tile_w;
    int tile_h;
    
    int tile_preview_w; //width of tiles in tile selecting
    int tile_preview_h;
    
    int tile_zoom_pixels;
    struct Camera camera;
    
    int cursor_x;
    int cursor_y;
    
    int play_area_left;
    int play_area_right;
    int play_area_top;
    int play_area_bottom;
    
    bool should_redraw;
};


void editor_init();
void editor_destroy();
void editor_update();
void _editor_update_play_area();
void _editor_update_select_tile();
void _editor_update_play_area_size();

//returns true if should_redraw was true and a draw happened
bool editor_draw();

//returns a pointer to the editor. Nonudat encapsulation stuff
struct Editor* editor_get();
struct Bitmap* editor_get_bitmap();

void editor_set_key(const int code, const uint8_t state);

int editor_get_key(const int code);

void editor_add_key_state(const int code, const uint8_t state);

void editor_remove_key_state(const int code, const uint8_t state);

void editor_move_cursor_play_area(int max_x, int max_y);

void editor_move_cursor_tile_select();

//uses cursor_x and cursor_y
void editor_set_tile(const int new_id);

void editor_menu_callback(int item_id);

void editor_tilemap_zoom(int zoom_dir);

#endif