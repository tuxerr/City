#include "spaceship.h"

Spaceship::Spaceship(Object *o) :
    object(o), speed(0.3)
{
    
}

void Spaceship::move(bool up, bool down, bool right, bool left) {
    if(up) {
        object->rotate(PITCH_SPEED,1,0,0);
    } else if(down) {
        object->rotate(-PITCH_SPEED,1,0,0);
    }

    if(right) {
        object->rotate(-ROLL_SPEED,0,1,0);
    } else if(left) {
        object->rotate(ROLL_SPEED,0,1,0);
    }

    object->translate(0,-speed,0);
}

void Spaceship::camera_config(Vec3<float> &position,Vec3<float> &direction, Vec3<float> &up_vector) {
    position=object->modelview_matrix()*Vec3<float>(0,4,1);
    direction=object->modelview_matrix()*Vec3<float>(0,-1,0);
    up_vector=object->modelview_matrix()*Vec3<float>(0,0,1)-object->position();
}
