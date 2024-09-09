/* date = July 9th 2024 8:51 pm */

#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

enum
{
    MOUSE_BUTTON_RELEASED,
    MOUSE_BUTTON_HELD
};


enum
{
    KEYCODE_RIGHT,
    KEYCODE_LEFT,
    KEYCODE_UP,
    KEYCODE_DOWN,
    KEYCODE_W,
    KEYCODE_A,
    KEYCODE_S,
    KEYCODE_D,
    
    KEYCODE_X,
    KEYCODE_Z,
    
    KEYCODE_RETURN,
    KEYCODE_BACKSPACE,
    KEYCODE_CTRL,
    KEYCODE_ADD,
    KEYCODE_SUBTRACT,
    
    KEYCODE_COUNT
};

enum
{
    KEYSTATE_RELEASED = 1,
    KEYSTATE_PRESSED = 1 << 1,
    KEYSTATE_JUST_PRESSED = 1 << 2
};


//TODO() use bitflags for the mouse buttons and just_moved instead of seperate variables
struct mouse_state
{
    int x;
    int y;
    uint8_t left_button;
    uint8_t right_button;
    bool just_moved;
};


struct keyboard_state
{
    uint32_t* keys;
};



#endif //INPUT_H
