#ifndef DEF_SPACESHIP
#define DEF_SPACESHIP

#include "vec3.hpp"
#include "object.hpp"

#define ROLL_SPEED 3
#define YAW_SPEED 3
#define PITCH_SPEED 3

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
