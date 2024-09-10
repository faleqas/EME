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
    
    editor->cursor.thickness = 4;
    
    editor->tile_w = 32;
    editor->tile_h = 32;
    
    const int map_w = 0;
    const int map_h = 0;
    game_map_create(&(editor->game_map), map_w, map_h);
    
    editor->asset_mng = asset_manager_create();
    //asset_manager_load_image(editor->asset_mng,
    //                         "block.jpeg", IMAGE_TEST);
    //
    //asset_manager_load_tile(editor->asset_mng,
    //                        TILE_TEST, IMAGE_TEST);
    //
    asset_manager_load_tiles_from_file(editor->asset_mng, "tiles.txt");
    //game_map_create(&(editor->game_map), m);

    get_client_rect(&(editor->client_w), &(editor->client_h));

    editor->pa.x = 0;
    editor->pa.y = 0;
    editor->pa.tiles_w = editor->client_w / editor->tile_w;
    editor->pa.tiles_h = 10;
    editor->pa.w = editor->pa.tiles_w * editor->tile_w;
    editor->pa.h = editor->pa.tiles_h * editor->tile_h;

    editor->camera.max_speed = 5.0f;
    editor->camera.accel = 0.5f;
    editor->camera.decel = 1.0f;
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
    if (Camera_update(&(editor->camera), &(editor->keyboard)))
    {
        editor->should_redraw = true;
    }

    if (editor->mouse.just_moved)
    {
        editor_move_cursor_mouse();
    }
    else
    {
        editor_move_cursor_keyboard();
    }

    if (editor_get_key(KEYCODE_X) & KEYSTATE_JUST_PRESSED)
    {
        editor_set_tile(1);
    }
    else if (editor_get_key(KEYCODE_Z) & KEYSTATE_JUST_PRESSED)
    {
        editor_set_tile(0);
    }
    if (editor->mouse.left_button == MOUSE_BUTTON_HELD)
    {
        editor_set_tile(1);
    }
    else if (editor->mouse.right_button == MOUSE_BUTTON_HELD)
    {
        editor_set_tile(0);
    }

    if (editor->should_redraw)
    {
        int pa_right = editor->pa.w + editor->pa.x;
        if (editor->cursor.x < editor->pa.x)
        {
            editor->cursor.x = editor->pa.x;
        }
        else if ((editor->cursor.x + editor->tile_w) > pa_right)
        {
            editor->cursor.x = pa_right - editor->tile_w;
        }
        
        int pa_bottom = editor->pa.h + editor->pa.y;
        if (editor->cursor.y < editor->pa.y)
        {
            editor->cursor.y = editor->pa.y;
        }
        else if ((editor->cursor.y + editor->tile_h) > pa_bottom)
        {
            editor->cursor.y = pa_bottom - editor->tile_h;
        }
    }
    
    editor->mouse.just_moved = false;
    
    for (int i = 0; i < KEYCODE_COUNT; i++)
    {
        editor->keyboard.keys[i] &= ~KEYSTATE_JUST_PRESSED;
    }
}


void editor_set_tile(int tile_id)
{
    int cursor_tx = editor->cursor.x / editor->tile_w;
    int cursor_ty = editor->cursor.y / editor->tile_h;

    game_map_set_tile(&(editor->game_map), cursor_tx, cursor_ty, tile_id);

    editor->should_redraw = true;
}


bool editor_move_cursor_keyboard()
{
    bool moved = false;
    
    if (editor_get_key(KEYCODE_RIGHT) & KEYSTATE_JUST_PRESSED)
    {
        editor->cursor.x += editor->tile_w;
        editor->should_redraw = true;
        moved = true;
    }
    else if (editor_get_key(KEYCODE_LEFT) & KEYSTATE_JUST_PRESSED)
    {
        editor->cursor.x -= editor->tile_w;
        editor->should_redraw = true;
        moved = true;
    }
    
    if (editor_get_key(KEYCODE_DOWN) & KEYSTATE_JUST_PRESSED)
    {
        editor->cursor.y += editor->tile_w;
        editor->should_redraw = true;
        moved = true;
    }
    else if (editor_get_key(KEYCODE_UP) & KEYSTATE_JUST_PRESSED)
    {
        editor->cursor.y -= editor->tile_w;
        editor->should_redraw = true;
        moved = true;
    }
    
    return moved;
}


void editor_move_cursor_mouse()
{
    int mx = editor->mouse.x;
    int my = editor->mouse.y;

    if (my > (editor->pa.h + editor->pa.y))
    {
        return;
    }
    mx += editor->camera.x;
    my += editor->camera.y;

    int tmx = mx / editor->tile_w;
    int tmy = my / editor->tile_h;
    
    int prev_cursor_x = editor->cursor.x;
    int prev_cursor_y = editor->cursor.y;
    
    editor->cursor.x = tmx * editor->tile_w;
    editor->cursor.y = tmy * editor->tile_h;
    
    if (prev_cursor_x != editor->cursor.x)
    {
        editor->should_redraw = true;
        //editor_set_tile(1);
    }
    else if (prev_cursor_y != editor->cursor.y)
    {
        editor->should_redraw = true;
        //editor_set_tile(1);
    }
}


bool editor_draw()
{
    //editor->should_redraw = true;
    if (editor->should_redraw) {
        
        struct Game_map* map = &(editor->game_map);
        struct Bitmap* bitmap = &(editor->bitmap);
        struct Cursor* cursor = &(editor->cursor);
        struct play_area_data* pa = &(editor->pa);
        
        bitmap_draw_rect(bitmap, 0, 0, bitmap->width, pa->h + pa->y + cursor->thickness, 50, 50, 70, 255);

        bitmap_draw_rect(bitmap, 0, pa->h + pa->y + cursor->thickness, bitmap->width, bitmap->height - (pa->h + pa->y), 30, 30, 40, 70);

        //draw tile selection
        int x = 0;
        int y = pa->h + pa->y + cursor->thickness;
        for (int i = 0; i < editor->asset_mng->_tiles_count; i++)
        {
            struct Tile* tile = editor->asset_mng->tiles + i;

            if (tile->image_id > 0)
            {
                draw_image(bitmap, asset_manager_get_image(editor->asset_mng, tile->image_id), x, y, editor->tile_w, editor->tile_h);
            }
            x += editor->tile_w;

            if ((x + editor->tile_w) > bitmap->width)
            {
                x = 0;
                y += editor->tile_h;
            }
        }
        
        //draw grid
        //tx means tile_x
        // for (int ty = 0; ty < pa->tiles_h; ty++)
        // {
        //     for (int tx = 0; tx < pa->tiles_w; tx++)
        //     {
        //         int x = (tx * editor->tile_w) + pa->x;
        //         int y = (ty * editor->tile_h) + pa->y;
                
        //         bitmap_draw_rect_outline(bitmap,
        //                                  x, y,
        //                                  editor->tile_w, editor->tile_h,
        //                                  150, 150, 150, 20,
        //                                  cursor->thickness);
        //     }
        // }

        for (int y = 0; y < map->tilemap_h; y++)
        {
            for (int x = 0; x < map->tilemap_w; x++)
            {
                int draw_x = (x * editor->tile_w) - editor->camera.x;
                int draw_y = (y * editor->tile_h) - editor->camera.y;

                if ((draw_x + editor->tile_w) < 0)
                {
                    continue;
                }
                if ((draw_x) > bitmap->width)
                {
                    continue;
                }
                if ((draw_y + editor->tile_h) < 0)
                {
                    continue;
                }
                if ((draw_y) > bitmap->height)
                {
                    continue;
                }

                if ((draw_y + editor->tile_h) <= (pa->h + pa->y))
                {
                    //draw grid tile
                    bitmap_draw_rect_outline(bitmap,
                                draw_x, draw_y,
                                editor->tile_w, editor->tile_h,
                                150, 150, 150, 20,
                                cursor->thickness);
                }
                else
                {
                    continue;
                }

                int tile_id = game_map_get_tile_by_pos(map, x, y);
                if (tile_id == 0) continue;

                struct Tile* tile = asset_manager_get_tile(editor->asset_mng, tile_id);
                if (tile == NULL) continue;

                draw_image(bitmap, asset_manager_get_image(editor->asset_mng, tile->image_id),
                draw_x, draw_y,
                editor->tile_w, editor->tile_h);
            }
        }
        
        bitmap_draw_rect_outline(bitmap,
                                 cursor->x - editor->camera.x, cursor->y - editor->camera.y,
                                 editor->tile_w, editor->tile_h,
                                 255, 255, 255, 255,
                                 cursor->thickness);
        
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
        
        case MI_EDITMAP:
        {
            create_dialog();
        } break;
    }
}