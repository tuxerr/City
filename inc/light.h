#ifndef DEF_LIGHT
#define DEF_LIGHT

#include <iostream>
#include "vec3.h"
#include "uniformblock.h"

class Light {
public:
    Light(UniformBlock *uniform,Vec3<float> pos,float intensity,Vec3<float> color);
    Light& operator=(const Light& l);
    void set_pos(Vec3<float> pos);
    void set_color(Vec3<float> color);
    void set_direction(Vec3<float> direction);    
    void set_spot(Vec3<float> direction,float illu_angle,float max_illu_angle);
    void desactivate_spot();
    void set_intensity(float intensity);
    void set_linear_dissipation(float linear_dissipation);
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
