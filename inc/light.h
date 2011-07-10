#ifndef DEF_LIGHT
#define DEF_LIGHT

#include <iostream>
#include "vec3.h"

class Light {
public:
    Light(UniformBlock *uniform,Vec3<float> pos,float intensity,Vec3<float> color);
    Light& operator=(const Light& l);
    void set_pos(Vec3<float> pos);
    void set_color(Vec3<float> color);
    void set_intensity(float intensity);
    void set_uniform(UniformBlock *uniform);
    static int uniform_size();

private:
    UniformBlock *uniform;
    Vec3<float> pos;
    Vec3<float> color;
    Vec3<float> direction;
    float intensity;
    float linear_dissipation;
    float max_illu_angle;
    float illu_angle;
};

#endif 
