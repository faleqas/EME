/* date = July 15th 2024 2:52 pm */

#ifndef CAMERA_H
#define CAMERA_H

#include "input.h"

struct Camera
{
    float x;
    float y;
    float velocity_x;
    float velocity_y;
    float accel; //acceleration
    float decel; //decelration
    float max_speed;
};

//returns true if the camera moved
bool Camera_update(struct Camera* camera, const struct keyboard_state* keys);

#endif //CAMERA_H
