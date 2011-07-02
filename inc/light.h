#ifndef DEF_LIGHT
#define DEF_LIGHT

#include <iostream>
#include "vec3.h"

class Light {
public:
    Light(Vec3<float> pos,float intensity,Vec3<float> color);
    void set_pos(Vec3<float> pos);
    void set_color(Vec3<float> color);
    void set_intensity(float intensity);

private:
    Vec3<float> pos;
    Vec3<float> color;
    float intensity;
};

#endif 
