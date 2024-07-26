#ifndef EME__BUTTON
#define EME__BUTTON

#include <stdbool.h>

#include "editor.h"


struct Button
{
    //must always be the first 4 elements of the struct to allow casting to SDL_Rect
    int x;
    int y;
    int w;
    int h;
    int text_w;
    int text_h;

    char* text;

    bool draw_outline; //true if mouse is on button
};

void Button_init_default(const struct Editor* editor, struct Button* button); //initializes default values for the button except the x and y and text
void Button_set_text(const struct Editor* editor, struct Button* button, const char* text);

void Button_mouse_moved(const struct Editor* editor, struct Button* button);
void Button_draw(const struct Editor* editor, const struct Button* button);

#endif