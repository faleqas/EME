#include "camera.h"
#include "input.h"

#include <stdint.h>

bool Camera_update(struct Camera* camera, const struct keyboard_state* keyboard)
{
    uint32_t* keys = keyboard->keys;
    int integer_x = (int)camera->x;
    int integer_y = (int)camera->y;

    if (keys[KEYCODE_D] & KEYSTATE_PRESSED)
    {
        camera->velocity_x += camera->accel;
        if (camera->velocity_x > camera->max_speed)
        {
            camera->velocity_x = camera->max_speed;
        }
    }
    else if (keys[KEYCODE_A] & KEYSTATE_PRESSED)
    {
        camera->velocity_x -= camera->accel;
        if (camera->velocity_x < -(camera->max_speed))
        {
            camera->velocity_x = -(camera->max_speed);
        }
    }
    else
    {
        if (camera->velocity_x > 0)
        {
            camera->velocity_x -= camera->decel;
            if (camera->velocity_x < 0)
            {
                camera->velocity_x = 0;
            }
        }
        else if (camera->velocity_x < 0)
        {
            camera->velocity_x += camera->decel;
            if (camera->velocity_x > 0)
            {
                camera->velocity_x = 0;
            }
        }
    }

    if (keys[KEYCODE_S] & KEYSTATE_PRESSED)
    {
        camera->velocity_y += camera->accel;
        if (camera->velocity_y > camera->max_speed)
        {
            camera->velocity_y = camera->max_speed;
        }
    }
    else if (keys[KEYCODE_W] & KEYSTATE_PRESSED)
    {
        camera->velocity_y -= camera->accel;
        if (camera->velocity_y < -(camera->max_speed))
        {
            camera->velocity_y = -(camera->max_speed);
        }
    }
    else
    {
        if (camera->velocity_y > 0)
        {
            camera->velocity_y -= camera->decel;
            if (camera->velocity_y < 0)
            {
                camera->velocity_y = 0;
            }
        }
        else if (camera->velocity_y < 0)
        {
            camera->velocity_y += camera->decel;
            if (camera->velocity_y > 0)
            {
                camera->velocity_y = 0;
            }
        }
    }

    camera->x += camera->velocity_x;
    camera->y += camera->velocity_y;

    if (integer_x != ((int)camera->x)) //we actually moved by a pixel at least
    {
        return true;
    }
    else if (integer_y != ((int)camera->y)) //we actually moved by a pixel at least
    {
        return true;
    }
    return false;
}