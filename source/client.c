#include <windows.h>
#include <malloc.h>
#include <memory.h>

#include "client.h"
#include "win32_layer.h"



void create_dialog()
{
    HWND window_handle = win32_get_window_handle();
    HWND dialog_handle = CreateWindowExA(0,
        "DIALOG_CLASS",
        (LPCSTR)"Dialog Box",
        WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        300,
        300,
        window_handle,
        NULL,
        NULL,
        NULL);
}


void get_client_rect(int* w, int* h)
{
    //WIN32
    HWND window_handle = win32_get_window_handle();
    RECT client_rect;
    GetClientRect(window_handle, &client_rect);
    
    int client_w = client_rect.right - client_rect.left;
    int client_h = client_rect.bottom - client_rect.top;
    
    if (w) {
        *w = client_w;
    }
    if (h) {
        *h = client_h;
    }
}


char* get_file_from_user(const char* title, bool open_only)
{
    HWND window_handle = win32_get_window_handle();
    OPENFILENAMEA file_dialog = {0};
    char* file_path = malloc(sizeof(char) * 512);
    file_path[0] = 0;
    
    file_dialog.lStructSize = sizeof(OPENFILENAMEA);
    file_dialog.lpstrFile = file_path;
    file_dialog.nMaxFile = 512;
    file_dialog.Flags = OFN_FILEMUSTEXIST;
    file_dialog.lpstrTitle = title;
    
    if (open_only) {
        GetOpenFileNameA(&file_dialog);
    }
    else {
        GetSaveFileNameA(&file_dialog);
    }
    //MessageBoxA(window_handle, file_path, "Client", MB_OK);
    
    return file_path;
}