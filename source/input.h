/* date = July 9th 2024 8:51 pm */

#ifndef INPUT_H
#define INPUT_H

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
    
    KEYCODE_RETURN,
    KEYCODE_BACKSPACE,
    KEYCODE_CTRL,
    KEYCODE_ADD,
    KEYCODE_SUBTRACT,
    
    KEYCODE_COUNT
};

enum
{
    KEYSTATE_RELEASED,
    KEYSTATE_PRESSED
};


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
    uint8_t* keys;
};



#endif //INPUT_H
