#ifndef DEF_LIGHT
#define DEF_LIGHT

#include <iostream>
#include "math.h"
#include "vec3.h"
#include "uniformblock.h"
#include "texture.h"
#include "matrix4.h"
#include "constants.h"

typedef enum Light_Types {
    OFF = 0,
    POINT_LIGHT = 1, 
    SPOT_LIGHT=2,
    DIRECTION_LIGHT=3
} Light_Types;

class Light {
public:
    Light(UniformBlock *uniform,float intensity,Vec3<float> color);
    void set_color(Vec3<float> color);
    void set_intensity(float intensity);
    void set_linear_dissipation(float linear_dissipation);
    void set_uniform(UniformBlock *uniform);
    void activate();
    void desactivate();
    Matrix4& get_matrix();

protected:
    UniformBlock *uniform;
    int type;
    Vec3<float> color;
    float intensity;
    float linear_dissipation;
    Texture shadowmap;
    Matrix4 projection_camera_matrix;
};



class PointLight : public Light {
public:
    PointLight(UniformBlock *uniform,Vec3<float> pos,float intensity,Vec3<float> color);
    void set_pos(Vec3<float> pos);
    void set_uniform(UniformBlock *uniform);

private:
    Vec3<float> pos;
};



class DirectionalLight : public Light{
public:
    DirectionalLight(UniformBlock *uniform,Vec3<float> direction,float intensity,Vec3<float> color);
    void set_direction(Vec3<float> direction);    
    void set_uniform(UniformBlock *uniform);

private:
    Vec3<float> direction;
};



class SpotLight : public PointLight {
public:
    SpotLight(UniformBlock *uniform,Vec3<float> pos,Vec3<float> direction,float intensity,Vec3<float> color,float illu_angle,float max_illu_angle);
    void set_spot(float illu_angle,float max_illu_angle);
    void set_direction(Vec3<float> direction);
    void set_uniform(UniformBlock *uniform);
    
private:
    Vec3<float> direction;
    float max_illu_angle;
    float illu_angle;
}; 



#endif 
