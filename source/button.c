#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#include "button.h"


void Button_init_default(const struct Editor* editor, struct Button* button)
{
    button->w = 36 * 4;
    button->h = 18 * 4;
    
    /*
    memcpy(&(button->color), &(editor->background_color), sizeof(SDL_Color));
    button->color.r -= 20;
    button->color.g -= 20;
    button->color.b -= 20;
    
    button->text_color.r = 0;
    button->text_color.g = 0;
    button->text_color.b = 0;
    button->text_color.a = 255;
    
    button->outline_color.r = 0;
    button->outline_color.g = 0;
    button->outline_color.b = 0;
    button->outline_color.a = 255;
    
    button->text = NULL;
*/
}


void Button_set_text(const struct Editor* editor, struct Button* button, const char* text)
{
    if (!text) {
        return;
    }
    
    if (button->text) {
        free(button->text);
    }
    
    // size_t len = strlen(text) + 1;
    
    // button->text = malloc(sizeof(char) * len);
    // memcpy(button->text, text, sizeof(char) * len);
    
    // TTF_SizeText(editor->font, text, &(button->text_w), &(button->text_h));\
    // button->w = (int)(button->text_w * 1.1f);
    // button->h = (int)(button->text_h * 1.1f);
    
    // printf("%s\n", button->text);
}


void Button_draw(const struct Editor* editor, const struct Button* button)
{
    if (button->draw_outline) {
    }
    
    if (button->text) {
        int text_x = (button->x + (button->w / 2)) - (button->text_w / 2);
        int text_y =  (button->y + (button->h / 2)) - (button->text_h / 2);
    }
    
}
void Button_mouse_moved(const struct Editor* editor, struct Button* button)
{
    const struct mouse_state* mouse = &(editor->mouse);
    
    bool in_x = (mouse->x > button->x) && (mouse->x < (button->x + button->w));
    bool in_y = (mouse->y > button->y) && (mouse->y < (button->y + button->h));
    
    if (in_x && in_y) {
        button->draw_outline = true;
    }
    else {
        button->draw_outline = false;
    }
}