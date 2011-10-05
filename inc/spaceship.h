#ifndef DEF_SPACESHIP
#define DEF_SPACESHIP

#include "vec3.h"
#include "object.h"

#define ROLL_SPEED 1
#define YAW_SPEED 1
#define PITCH_SPEED 1

class Spaceship {
public:
    Spaceship(Object *o);
    Vec3<float> get_position();
    Vec3<float> get_direction();
    void move(bool up, bool down, bool right, bool left);
    void camera_config(Vec3<float> &position,Vec3<float> &direction, Vec3<float> &up_vector);
    
private:
    Object *object;
    float speed;
};

#endif
