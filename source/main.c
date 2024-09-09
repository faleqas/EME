// main.c : This file contains the 'main' function. Program execution begins and ends there.


/*************

win32 stuff and main program loop

*************/


#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <memory.h>
#include <assert.h>

#include <windows.h>
#include <windowsx.h>

#include "editor.h"
#include "defines.h"
#include "bitmap.h"
#include "input.h"
#include "client.h"


global const char* window_class_name = "EME_WINDOW_CLASS";
global const char* window_menu_name = "EME_MENU";

global BITMAPINFO win32_bitmap_info = {0};
global struct Bitmap* bitmap = NULL;
global HBITMAP hbitmap = NULL;
global HDC bitmap_dc = NULL;

//g_ = global
//since window_handle gets passed to some functions
global HWND g_window_handle = NULL;

internal LRESULT CALLBACK
win32_window_callback(HWND window_handle,
                      UINT message_code,
                      WPARAM w_param,
                      LPARAM l_param
                      );

internal void
win32_resize_dib_section(const int client_w, const int client_h);

internal void
win32_update_window(HDC device_context,
                    const int x, const int y,
                    const int w, const int h);

internal HMENU
win32_create_menu();

internal LRESULT CALLBACK
win32_dialogbox_callback(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param);

internal int
get_keycode_from_w_param(int w_param);


LARGE_INTEGER frequency;
float target_seconds_per_frame = 1.0f / 60.0f;

float get_seconds_per_frame(LARGE_INTEGER start_counter,
                            LARGE_INTEGER end_counter)
{
    
    return ((float)(end_counter.QuadPart - start_counter.QuadPart) / (float)frequency.QuadPart);
}



int CALLBACK
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nShowCmd
        )
{
    
    WNDCLASS window_class = {0};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = &win32_window_callback;
    window_class.hInstance = hInstance;
    window_class.lpszMenuName = window_menu_name;
    window_class.lpszClassName = window_class_name;
    //window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    
    if (!RegisterClassA(&window_class)) {
        //FAIL()
        return 1;
    }
    
    //menu
    HMENU menu = win32_create_menu();
    
    g_window_handle = CreateWindowExA(0,
                                      window_class_name,
                                      (LPCSTR)"EME",
                                      WS_CAPTION | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      SCREEN_WIDTH,
                                      SCREEN_HEIGHT,
                                      NULL,
                                      menu,
                                      hInstance,
                                      NULL);
    if (!g_window_handle) {
        //FAIL()
        return 2;
    }

    editor_init();
    struct Editor* editor = editor_get();
    bitmap = editor_get_bitmap();
    editor->should_redraw = true;
    
    MSG message;
    bool running = true;
    HDC dc = GetDC(g_window_handle);
    
    RECT client_rect;
    GetClientRect(g_window_handle, &client_rect);
    
    int client_w = client_rect.right - client_rect.left;
    int client_h = client_rect.bottom - client_rect.top;

    win32_resize_dib_section(client_w, client_h);
    
    WNDCLASS dialog_window_class = { 0 };
    dialog_window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    dialog_window_class.lpfnWndProc = &win32_dialogbox_callback;
    dialog_window_class.hInstance = hInstance;
    dialog_window_class.lpszClassName = "DIALOG_CLASS";
    
    if (!RegisterClassA(&dialog_window_class)) {
        //FAIL()
        return 3;
    }

    LARGE_INTEGER start_counter, end_counter, counts, fps, ms;
    
    QueryPerformanceCounter(&start_counter);
    
    QueryPerformanceFrequency(&frequency);
    
    while (running)
    {
        bool peeked = false;
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            peeked = true;
            if (message.message == WM_QUIT) {
                if (message.wParam == 1) {
                    //DestroyWindow(dialog_handle);
                }
                else {
                    running = false;
                }
            }
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        QueryPerformanceCounter(&end_counter);
        float seconds_per_frame = get_seconds_per_frame(start_counter,
                                                end_counter);
        if(seconds_per_frame < target_seconds_per_frame)
        {
            DWORD sleep_ms;
            
            sleep_ms = (DWORD)(1000 * (target_seconds_per_frame - seconds_per_frame));
            
            Sleep(sleep_ms);
            
            while(seconds_per_frame < target_seconds_per_frame)
            {
                QueryPerformanceCounter(&end_counter);
                
                seconds_per_frame = get_seconds_per_frame(start_counter,
                                                          end_counter);
            }
        }

        QueryPerformanceCounter(&end_counter);
        
        seconds_per_frame = get_seconds_per_frame(start_counter,
                                                  end_counter);
        
        start_counter = end_counter;
        
        editor_update();
        
        if (editor_draw()) {
            //draw bitmap to screen
            win32_update_window(dc, 0, 0, client_w, client_h);
        }
    }
    
    if (PROGRAM_CLEANUP) {
        if (bitmap->data) {
            VirtualFree(bitmap->data, 0, MEM_RELEASE);
        }
        editor_destroy();
    }
    DestroyWindow(g_window_handle);
    
    return 0;
}


internal HMENU
win32_create_menu()
{
    HMENU menu = CreateMenu();
    AppendMenuA(menu,
                MF_STRING,
                MI_LOADMAP,
                "Load Map");
    AppendMenuA(menu,
                MF_STRING,
                MI_SAVEMAP,
                "Save Map");
    AppendMenuA(menu,
                MF_STRING,
                MI_EDITMAP,
                "Edit Map");
    AppendMenuA(menu,
                MF_STRING,
                MI_CLEARMAP,
                "Clear Map");
    return menu;
}


HWND win32_get_window_handle()
{
    return g_window_handle;
}


internal LRESULT CALLBACK
win32_window_callback(HWND window_handle,
                      UINT message_code,
                      WPARAM w_param,
                      LPARAM l_param
                      )
{
    LRESULT result = 0;
    
    switch (message_code)
    {
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;
        
        case WM_COMMAND:
        {
            editor_menu_callback(w_param);
            
        } break;
        
        case WM_MOUSEMOVE:
        {
            struct Editor* editor = editor_get();
            if (editor)
            {
                editor->mouse.x = l_param & 0xFFFF;
                editor->mouse.y = l_param >> 16;
                editor->mouse.just_moved = true;
                //editor->cursor.x = editor->mouse.x;
                //editor->cursor.y = editor->mouse.y;
                //editor->should_redraw = true;
            }
        } break;
        
        case WM_KEYDOWN:
        {
            int code = get_keycode_from_w_param(w_param);
            
            if (code != -1) {
                if (!(editor_get_key(code) & KEYSTATE_PRESSED)) {
                    editor_add_key_state(code, KEYSTATE_JUST_PRESSED);
                }
                editor_add_key_state(code, KEYSTATE_PRESSED);
            }
        } break;
        
        case WM_KEYUP:
        {
            int code = get_keycode_from_w_param(w_param);
            
            if (code != -1) {
                editor_remove_key_state(code, KEYSTATE_PRESSED);
            }
        } break;
        
        case WM_LBUTTONDOWN:
        {
            struct Editor* editor = editor_get();
            if (editor)
            {
                editor->mouse.left_button = MOUSE_BUTTON_HELD;
            }
        } break;
        
        case WM_LBUTTONUP:
        {
            struct Editor* editor = editor_get();
            if (editor)
            {
                editor->mouse.left_button = MOUSE_BUTTON_RELEASED;
            }
        } break;
        
        
        case WM_RBUTTONDOWN:
        {
            struct Editor* editor = editor_get();
            if (editor)
            {
                editor->mouse.right_button = MOUSE_BUTTON_HELD;
            }
        } break;
        
        case WM_RBUTTONUP:
        {
            struct Editor* editor = editor_get();
            if (editor)
            {
                editor->mouse.right_button = MOUSE_BUTTON_RELEASED;
            }
        } break;
        
        case WM_SIZE:
        {
            RECT client_rect;
            GetClientRect(window_handle, &client_rect);
            
            int w = client_rect.right - client_rect.left;
            int h = client_rect.bottom - client_rect.top;
            
            win32_resize_dib_section(w, h);
            struct Editor* editor = editor_get();
            if (editor)
            {
                editor->should_redraw = true;
            }
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint_info;
            HDC device_context = BeginPaint(window_handle, &paint_info);
            HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
            //FillRect(device_context, &paint_info.rcPaint, (HBRUSH) (brush));

            {
                int x = paint_info.rcPaint.left;
                int y = paint_info.rcPaint.top;
                
                int w = paint_info.rcPaint.right - x;
                int h = paint_info.rcPaint.bottom - y;
                
                //win32_update_window(device_context, x, y, w, h);
            }
            EndPaint(window_handle, &paint_info);
        } break;
        
        default:
        {
            result = DefWindowProcA(window_handle, message_code, w_param, l_param);
        } break;
    };
    
    return result;
}


internal void
win32_resize_dib_section(const int client_w, const int client_h)
{
    if (!bitmap) return; //did not initial editor yet
    if (!hbitmap)
    {
        bitmap_dc = CreateCompatibleDC(GetDC(g_window_handle));
    }
    else {
        DeleteObject(hbitmap);
        DeleteObject(bitmap_dc);
    }
    
    BITMAPINFOHEADER* header = &(win32_bitmap_info.bmiHeader);
    int bpp = bitmap->bytes_per_pixel * 8;
    
    header->biSize = sizeof(BITMAPINFOHEADER);
    header->biWidth = client_w;
    header->biHeight = -client_h;
    header->biPlanes = 1;
    header->biBitCount = bpp;
    header->biCompression = BI_RGB;
    
    const size_t bitmap_memsize = client_w * client_h * bitmap->bytes_per_pixel;
    
    hbitmap = CreateDIBSection(bitmap_dc, header, DIB_RGB_COLORS, &(bitmap->data), NULL, 0);
    SelectObject(bitmap_dc, hbitmap);
    
    bitmap->width = client_w;
    bitmap->height = client_h;
    
    //bitmap_clear(&bitmap, 0x00, 0x00, 0x00);
}


internal void
win32_update_window(HDC device_context,
                    const int x, const int y,
                    const int w, const int h)
{
    BLENDFUNCTION bf = {0};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 0xff;
    bf.AlphaFormat = AC_SRC_ALPHA;

    // if (!AlphaBlend(device_context, 0, 0, w, h, bitmap_dc, 0, 0, bitmap->width, bitmap->height, bf))
    // {
    //     return;
    // }

    StretchDIBits(device_context,
                  0, 0, bitmap->width, bitmap->height,
                  0, 0, w, h,
                  bitmap->data, &win32_bitmap_info,
                  DIB_RGB_COLORS,
                  SRCCOPY);

    RECT rect;
    rect.left = 0;
    rect.top = 100;
}


internal LRESULT CALLBACK
win32_dialogbox_callback(HWND handle, UINT msg, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch (msg)
    {
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
        {
            PostQuitMessage(1);
            DestroyWindow(handle);
        } break;
        
        default:
        {
            result = DefWindowProcA(handle, msg, w_param, l_param);
        } break;
    }
    
    return result;
}


internal int
get_keycode_from_w_param(int w_param)
{
    switch (w_param)
    {
        case 'A': {
            return KEYCODE_A;
        } break;
        
        case 'D': {
            return KEYCODE_D;
        } break;
        
        case 'W': {
            return KEYCODE_W;
        } break;
        
        case 'S': {
            return KEYCODE_S;
        } break;
        
        case 'X': {
            return KEYCODE_X;
        } break;

        case 'Z': {
            return KEYCODE_Z;
        } break;
        
        case VK_RIGHT: {
            return KEYCODE_RIGHT;
        } break;
        
        case VK_LEFT: {
            return KEYCODE_LEFT;
        } break;
        
        case VK_DOWN: {
            return KEYCODE_DOWN;
        } break;
        
        case VK_UP: {
            return KEYCODE_UP;
        } break;
        
        case VK_RETURN: {
            return KEYCODE_RETURN;
        } break;
        
        case VK_BACK: {
            return KEYCODE_BACKSPACE;
        } break;
        
        case VK_CONTROL: {
            return KEYCODE_CTRL;
        } break;
        
        case VK_ADD: {
            return KEYCODE_ADD;
        } break;
        
        case VK_SUBTRACT: {
            return KEYCODE_SUBTRACT;
        } break;
        
        default: break;
    }
    
    return -1;
}