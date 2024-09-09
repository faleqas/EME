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
    MI_CLEARMAP,
    MI_EDITMAP
};


//EDITOR STATES
enum
{
    E_PLAY_AREA,
    E_SELECT_TILE
};


struct play_area_data
{
    int x;
    int y;
    int w;
    int h;
    
    int tiles_w; //width in tiles
    int tiles_h; //height in tiles
};


struct Editor
{
    int state;
    int client_w;
    int client_h;
    
    int tile_w;
    int tile_h;
    
    struct AssetManager* asset_mng;
    
    struct mouse_state mouse;
    struct keyboard_state keyboard;
    
    //bitmap pixel data is allocated and freed by the platform layer
    //since calling the specific allocation functions for the OS
    //is faster than malloc()
    //(as if we care about speed)
    struct Bitmap bitmap;
    struct Game_map game_map;
    
    struct Cursor cursor;
    
    struct play_area_data pa;

    struct Camera camera;
    
    bool should_redraw;
};


void editor_init();
void editor_destroy();
void editor_update();

bool editor_move_cursor_keyboard();
void editor_move_cursor_mouse();

void editor_set_tile(int tile_id); //sets the tile the cursor is currently pointing

//returns true if should_redraw was true and a draw happened
bool editor_draw();

//returns a pointer to the editor. Nonudat encapsulation stuff
struct Editor* editor_get();
struct Bitmap* editor_get_bitmap();

void editor_set_key(const int code, const uint8_t state);

int editor_get_key(const int code);

void editor_add_key_state(const int code, const uint8_t state);

void editor_remove_key_state(const int code, const uint8_t state);

void editor_menu_callback(int item_id);

#endif