#include <memory.h>
#include <malloc.h>
#include <assert.h>

#include <stb_image.h>

#include <windows.h>

#include "editor.h"
#include "defines.h"
#include "input.h"
#include "bitmap.h"
#include "client.h"
#include "camera.h"
#include "game_map.h"
#include "image.h"

//TODO(omar): zoom in using the cursor as origin not (0, 0)


global struct Editor* editor;



void editor_init()
{
    editor = calloc(1, sizeof(struct Editor));
    assert(editor);
    editor->state = E_PLAY_AREA;
    editor->bitmap.bytes_per_pixel = 4;
    editor->keyboard.keys = calloc(KEYCODE_COUNT, sizeof(uint32_t));
    
    const int map_w = 0;
    const int map_h = 0;
    game_map_create(&(editor->game_map), map_w, map_h);
    
    editor->tile_w = 32;
    editor->tile_h = 32;
    
    editor->tile_preview_w = 32;
    editor->tile_preview_h = 32;
    
    editor->tile_zoom_pixels = 4;
    editor->play_area_left = editor->tile_w * 2 + 20;
    editor->play_area_top = editor->tile_h * 2 + 20;
    
    editor->asset_mng = asset_manager_create();
    asset_manager_load_image(editor->asset_mng,
                             "block.jpeg", IMAGE_TEST);
    
    asset_manager_load_tile(editor->asset_mng,
                            TILE_TEST, IMAGE_TEST);
    editor->cursor.thickness = 4;
    //game_map_create(&(editor->game_map), m);
}

void editor_destroy()
{
    //game_map_save(&(editor->game_map), "map.dat");
    if (editor) {
        asset_manager_destroy(editor->asset_mng);
        game_map_delete(&(editor->game_map));
        free(editor->keyboard.keys);
        free(editor);
    }
}


void editor_update()
{
    _editor_update_play_area_size();
    
    if (editor_get_key(KEYCODE_CTRL) & KEYSTATE_PRESSED) {
        if (editor_get_key(KEYCODE_S) & KEYSTATE_JUST_PRESSED) {
            if (editor->state == E_PLAY_AREA) {
                editor->state = E_SELECT_TILE;
                editor->cursor.x = 0;
                editor->cursor.y = 0;
            }
            else {
                editor->state = E_PLAY_AREA;
                editor->cursor.x = 0;
                editor->cursor.y = 0;
            }
        }
    }
    
    switch (editor->state)
    {
        case E_PLAY_AREA:
        {
            _editor_update_play_area();
        } break;
        
        case E_SELECT_TILE:
        {
            _editor_update_select_tile();
        } break;
    };
    
    //memset(editor->keyboard.keys, 0, sizeof(uint8_t) * KEYCODE_COUNT);
    editor->mouse.just_moved = false;
    
    for (int i = 0; i < KEYCODE_COUNT; i++)
    {
        editor->keyboard.keys[i] &= ~KEYSTATE_JUST_PRESSED;
    }
}


void _editor_update_play_area()
{
    editor_move_cursor_play_area(editor->play_area_right, editor->play_area_bottom);
    
    if (editor_get_key(KEYCODE_RETURN) == KEYSTATE_PRESSED) {
        editor_set_tile(TILE_TEST);
    }
    else if (editor_get_key(KEYCODE_BACKSPACE) == KEYSTATE_PRESSED) {
        editor_set_tile(TILE_NONE);
    }
    
    else if (editor->mouse.left_button == MOUSE_BUTTON_HELD) {
        editor_set_tile(TILE_TEST);
    }
    
    else if (editor->mouse.right_button == MOUSE_BUTTON_HELD) {
        editor_set_tile(TILE_NONE);
    }
    
    if (editor_get_key(KEYCODE_RIGHT) & KEYSTATE_JUST_PRESSED) {
        editor->camera.tile_x++;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_LEFT) & KEYSTATE_JUST_PRESSED) {
        editor->camera.tile_x--;
        editor->should_redraw = true;
    }
    if (editor_get_key(KEYCODE_UP) & KEYSTATE_JUST_PRESSED) {
        editor->camera.tile_y--;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_DOWN) & KEYSTATE_JUST_PRESSED) {
        editor->camera.tile_y++;
        editor->should_redraw = true;
    }
    
    if (editor_get_key(KEYCODE_ADD) & KEYSTATE_JUST_PRESSED) {
        editor_tilemap_zoom(1);
    }
    else if (editor_get_key(KEYCODE_SUBTRACT) & KEYSTATE_JUST_PRESSED) {
        editor_tilemap_zoom(-1);
    }
}


void _editor_update_select_tile()
{
    /*editor->cursor.x = 0;
    editor->cursor.y = 0;*/
    editor_move_cursor_tile_select();
}


void _editor_update_play_area_size()
{
    int client_w = 0;
    int client_h = 0;
    get_client_rect(&client_w, &client_h);
    editor->play_area_right = client_w - 100;
    editor->play_area_bottom = client_h - 100;
    
    editor->client_w = client_w;
    editor->client_h = client_h;
}


bool editor_draw()
{
    if (editor->should_redraw) {
        //draw tilemap
        struct Game_map* map = &(editor->game_map);
        struct Bitmap* bitmap = &(editor->bitmap);
        bitmap_clear(bitmap, 0x99, 0x99, 0x99);
        bitmap_draw_rect(bitmap,
                         editor->play_area_left,
                         editor->play_area_top,
                         editor->play_area_right - editor->play_area_left,
                         editor->play_area_bottom - editor->play_area_top,
                         0x22, 0x22, 0x22);
        
        { //drawing the tilemap
            int id = -1;
            int x = -1;
            int y = -1;
            
            struct Camera* camera = &(editor->camera);
            
            struct Tile* tile = NULL;
            struct Image* img = NULL;
            for (int i = 0; i < map->tile_count; i++) 
            {
                id = game_map_get_tile_by_index(map, i, &x, &y);
                
                int draw_x = ((x - camera->tile_x) * editor->tile_w) + editor->play_area_left;
                int draw_y = ((y - camera->tile_y) * editor->tile_h) + editor->play_area_top;
                
                bool in_play_area = true;
                
                if (draw_x < editor->play_area_left) {
                    in_play_area = false;
                }
                if (draw_y < editor->play_area_top) {
                    in_play_area = false;
                }
                if (draw_x > editor->play_area_right) {
                    in_play_area = false;
                }
                if (draw_y + editor->tile_h > editor->play_area_bottom) {
                    in_play_area = false;
                }
                
                if (!in_play_area) {
                    continue;
                }
                
                int w = editor->tile_w;
                if (draw_x + editor->tile_w > editor->play_area_right) {
                    w = editor->play_area_right - draw_x;
                }
                
                if (id) {
                    tile = asset_manager_get_tile(editor->asset_mng,
                                                  id);
                    if (!tile) {
                        continue;
                    }
                    
                    img = asset_manager_get_image(editor->asset_mng,
                                                  tile->image_id);
                    if (!img) {
                        continue;
                    }
                    
                    
                    draw_image(bitmap, img,
                               draw_x,
                               draw_y,
                               w, editor->tile_h);
                }
                else {
                    bitmap_draw_rect(bitmap,
                                     draw_x, draw_y,
                                     w, editor->tile_h,
                                     0x44, 0x44, 0x44);
                }
            }
        }
        
        int cursor_x = editor->cursor.x;
        int cursor_y = editor->cursor.y;
        int cursor_w = editor->tile_w;
        int cursor_h = editor->tile_h;
        
        if (editor->state == E_PLAY_AREA) {
            cursor_x += editor->play_area_left;
            cursor_y += editor->play_area_top;
            
            //cursor_w = editor->tile_preview_w;
            //cursor_h = editor->tile_preview_h;
        }
        else if (editor->state == E_SELECT_TILE) {
            cursor_w = editor->tile_preview_w;
            cursor_h = editor->tile_preview_h;
        }
        
        bitmap_draw_rect_outline(bitmap,
                                 cursor_x, cursor_y,
                                 cursor_w, cursor_h,
                                 0xff, 0xff, 0xff,
                                 editor->cursor.thickness);
        
        bitmap_draw_rect_outline(bitmap,
                                 editor->play_area_left, editor->play_area_top,
                                 (editor->play_area_right - editor->play_area_left),
                                 (editor->play_area_bottom - editor->play_area_top),
                                 0xFF, 0xFF, 0x00,
                                 4);
        
        
        for (int i = 0; i < editor->asset_mng->_tiles_count; i++)
        {
            struct Tile* tile = editor->asset_mng->tiles + i;
            struct Image* img = asset_manager_get_image(editor->asset_mng,
                                                        tile->image_id);
            draw_image(bitmap, img,
                       0,
                       0,
                       editor->tile_preview_w,
                       editor->tile_preview_h);
        }
        
        
        editor->should_redraw = false;
        
        return true;
    }
    else {
        return false;
    }
}


struct Editor* editor_get()
{
    return editor;
}

struct Bitmap* editor_get_bitmap()
{
    return &(editor->bitmap);
}


void editor_set_key(const int code, const uint8_t state)
{
    if ((code >= 0) && (code < KEYCODE_COUNT)) {
        editor->keyboard.keys[code] = state;
    }
}


int editor_get_key(const int code)
{
    if ((code >= 0) && (code < KEYCODE_COUNT)) {
        return editor->keyboard.keys[code];
    }
    
    return -1;
}


void editor_add_key_state(const int code, const uint8_t state)
{
    if ((code >= 0) && (code < KEYCODE_COUNT)) {
        editor->keyboard.keys[code] |= state;
    }
}


void editor_remove_key_state(const int code, const uint8_t state)
{
    if ((code >= 0) && (code < KEYCODE_COUNT)) {
        editor->keyboard.keys[code] &= ~state;
    }
}


void editor_move_cursor_play_area(int max_x, int max_y)
{
    int prev_x = editor->cursor.x;
    int prev_y = editor->cursor.y;
    
    if (editor_get_key(KEYCODE_D) & KEYSTATE_JUST_PRESSED) {
        editor->cursor.x += editor->tile_w;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_A) & KEYSTATE_JUST_PRESSED) {
        editor->cursor.x -= editor->tile_w;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_S) & KEYSTATE_JUST_PRESSED) {
        editor->cursor.y += editor->tile_h;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_W) & KEYSTATE_JUST_PRESSED) {
        editor->cursor.y -= editor->tile_h;
        editor->should_redraw = true;
    }
    else if (editor->mouse.just_moved) {
        int mouse_tile_x = (editor->mouse.x - editor->play_area_left) / editor->tile_w;
        int mouse_tile_y = (editor->mouse.y - editor->play_area_top) / editor->tile_h;
        int cursor_tile_x = (editor->cursor.x) / editor->tile_w;
        int cursor_tile_y = editor->cursor.y / editor->tile_h;
        
        if ((mouse_tile_x != cursor_tile_x) || (mouse_tile_y != cursor_tile_y)) {
            editor->cursor.x = mouse_tile_x * editor->tile_w;
            editor->cursor.y = mouse_tile_y * editor->tile_h;
            if (editor->cursor.x + editor->tile_w > editor->play_area_right) {
                editor->cursor.x = cursor_tile_x * editor->tile_w;
            }
            else if (editor->cursor.y + editor->tile_w > SCREEN_HEIGHT) {
                editor->cursor.x = cursor_tile_y * editor->tile_w;
            }
            else {
                editor->should_redraw = true;
            }
        }
    }
    
    if (editor->cursor.x < 0) {
        editor->cursor.x = prev_x;
        if (editor->mouse.just_moved == false) {
            editor->camera.tile_x--;
        }
    }
    else if (((editor->cursor.x + editor->play_area_left) + editor->tile_w) > max_x) {
        editor->cursor.x = prev_x;
        if (editor->mouse.just_moved == false) {
            editor->camera.tile_x++;
        }
    }
    
    if (editor->cursor.y < 0) {
        editor->cursor.y = prev_y;
        if (editor->mouse.just_moved == false) {
            editor->camera.tile_y--;
        }
    }
    else if (((editor->cursor.y + editor->play_area_top) + editor->tile_h) > max_y) {
        editor->cursor.y = prev_y;
        if (editor->mouse.just_moved == false) {
            editor->camera.tile_y++;
        }
    }
}


void editor_move_cursor_tile_select()
{
    int move_x = 0;
    int move_y = 0;
    int cursor_w = editor->tile_preview_w;
    int cursor_h = editor->tile_preview_h;
    
    if (editor_get_key(KEYCODE_D) & KEYSTATE_JUST_PRESSED) {
        move_x = editor->tile_preview_w;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_A) & KEYSTATE_JUST_PRESSED) {
        move_x = -editor->tile_preview_w;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_S) & KEYSTATE_JUST_PRESSED) {
        move_y = editor->tile_preview_h;
        editor->should_redraw = true;
    }
    else if (editor_get_key(KEYCODE_W) & KEYSTATE_JUST_PRESSED) {
        move_y = -editor->tile_preview_h;
        editor->should_redraw = true;
    }
    
    editor->cursor.x += move_x;
    editor->cursor.y += move_y;
    
    if (editor->cursor.x < 0) {
        editor->cursor.x -= move_x;
    }
    else if (editor->cursor.x + cursor_w > (editor->play_area_left)) {
        editor->cursor.x -= move_x;
    }
    
    if (editor->cursor.y < 0) {
        editor->cursor.y -= move_y;
    }
    else if ((editor->cursor.y + cursor_h) > editor->client_h) {
        editor->cursor.y -= move_y;
    }
}


void editor_set_tile(const int new_id)
{
    int tile_x = editor->cursor.x / editor->tile_w;
    int tile_y = editor->cursor.y / editor->tile_h;
    tile_x += editor->camera.tile_x;
    tile_y += editor->camera.tile_y;
    
    game_map_set_tile(&(editor->game_map),
                      tile_x, tile_y,
                      new_id);
    editor->should_redraw = true;
}


void editor_menu_callback(int item_id)
{
    switch (item_id)
    {
        case MI_SAVEMAP:
        {
            //char* path = get_file_from_user("Open Game Map");
            //free(path);
            char* path = get_file_from_user("Save Map", false);
            game_map_save(&(editor->game_map), path);
            editor->should_redraw = true;
            free(path);
        } break;
        
        case MI_LOADMAP:
        {
            char* path = get_file_from_user("Load Map", true);
            game_map_load(&(editor->game_map), path);
            editor->should_redraw = true;
            free(path);
        } break;
        
        case MI_CLEARMAP:
        {
            game_map_clear(&(editor->game_map));
            editor->should_redraw = true;
        } break;
    }
}


void editor_tilemap_zoom(int zoom_dir)
{
    int cursor_tile_x = (editor->cursor.x) / editor->tile_w;
    int cursor_tile_y = editor->cursor.y / editor->tile_h;
    
    editor->tile_w += editor->tile_zoom_pixels * zoom_dir;
    editor->tile_h += editor->tile_zoom_pixels * zoom_dir;
    if (editor->tile_w < 16) {
        editor->tile_w = 16;
    }
    if (editor->tile_h < 16) {
        editor->tile_h = 16;
    }
    
    if (editor->tile_w > 64) {
        editor->tile_w = 64;
    }
    if (editor->tile_h > 64) {
        editor->tile_h = 64;
    }
    
    editor->cursor.x = cursor_tile_x * editor->tile_w;
    editor->cursor.y = cursor_tile_y * editor->tile_h;
    
    if (((editor->cursor.x + editor->tile_w) + editor->play_area_left ) > editor->play_area_right) {
        editor->cursor.x = editor->play_area_right - editor->tile_w - editor->play_area_left;
    }
    if (((editor->cursor.y + editor->tile_h) + editor->play_area_top ) > editor->play_area_bottom) {
        editor->cursor.y = editor->play_area_bottom - editor->tile_h - editor->play_area_top;
    }
    
    editor->cursor.thickness = 4;
    editor->should_redraw = true;
}