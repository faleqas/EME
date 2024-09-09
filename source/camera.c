#include "camera.h"
#include "input.h"

#include <stdint.h>

bool Camera_update(struct Camera* camera, const struct keyboard_state* keyboard)
{
    uint32_t* keys = keyboard->keys;
    int integer_x = (int)camera->x;

    if (keys[KEYCODE_D] & KEYSTATE_PRESSED)
    {
        camera->velocity += camera->accel;
        if (camera->velocity > camera->max_speed)
        {
            camera->velocity = camera->max_speed;
        }
    }
    else if (keys[KEYCODE_A] & KEYSTATE_PRESSED)
    {
        camera->velocity -= camera->accel;
        if (camera->velocity < -(camera->max_speed))
        {
            camera->velocity = -(camera->max_speed);
        }
    }
    else
    {
        if (camera->velocity > 0)
        {
            camera->velocity -= camera->decel;
            if (camera->velocity < 0)
            {
                camera->velocity = 0;
            }
        }
        else if (camera->velocity < 0)
        {
            camera->velocity += camera->decel;
            if (camera->velocity > 0)
            {
                camera->velocity = 0;
            }
        }
    }

    camera->x += camera->velocity;

    if (integer_x != ((int)camera->x)) //we actually moved by a pixel at least
    {
        return true;
    }
    return false;
}