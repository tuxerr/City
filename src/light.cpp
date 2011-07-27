#include "light.h"

Light::Light(UniformBlock *uniform,float intensity,Vec3<float> color) : 
    uniform(uniform), type(OFF), color(color), intensity(intensity), linear_dissipation(1), 
    shadowmap(LIGHT_SHADOWMAP_HEIGHT,LIGHT_SHADOWMAP_WIDTH,TEXTURE_DEPTH)
{
    set_uniform(uniform);
}

void Light::set_color(Vec3<float> color) {
    this->color=color;
    uniform->set_value(color,"color");
}

void Light::set_intensity(float intensity) {
    this->intensity=intensity;
    uniform->set_value(intensity,"intensity");
}

void Light::activate() {
    uniform->set_value(type,"light_type");
}

void Light::desactivate() {
    uniform->set_value(OFF,"light_type");
}

void Light::set_linear_dissipation(float lin_dissipation) {
    this->linear_dissipation=lin_dissipation;
    Vec3<float> values(linear_dissipation,0,0);
    uniform->set_value(values,"spot_values");
}

void Light::set_uniform(UniformBlock *uniform) {
    this->uniform=uniform;
    uniform->set_value(type,"light_type");
    set_color(color);
    set_intensity(intensity);
    set_linear_dissipation(linear_dissipation);
}

Matrix4& Light::get_matrix() {
    return projection_camera_matrix;
}

/*** PointLight Class ***/

PointLight::PointLight(UniformBlock *uniform,Vec3<float> pos,float intensity,Vec3<float> color) : 
    Light(uniform,intensity,color) 
{
    type=POINT_LIGHT;
    set_pos(pos);
    uniform->set_value(type,"light_type");
}

void PointLight::set_pos(Vec3<float> pos) {
    this->pos=pos;
    uniform->set_value(pos,"origin");
}

void PointLight::set_uniform(UniformBlock *uniform) {
    Light::set_uniform(uniform);
    set_pos(pos);
}



/*** DirectionalLight ***/

DirectionalLight::DirectionalLight(UniformBlock *uniform,Vec3<float> direction,float intensity,Vec3<float> color) : 
    Light(uniform,intensity,color) 
{
    type=DIRECTION_LIGHT;
    set_direction(direction);
    uniform->set_value(type,"light_type");
}

void DirectionalLight::set_direction(Vec3<float> direction) {
    this->direction=direction;
    projection_camera_matrix.clear();
    projection_camera_matrix.perspective_ortho(10,NEAR,FAR*2,1);
    
    Matrix4 tmp;
    Vec3<float> dir_tmp = direction; dir_tmp.normalize();
    tmp.camera(Vec3<float>(0,0,0)-dir_tmp*FAR,direction,Vec3<float>(direction.y,direction.z,direction.x));

    projection_camera_matrix = projection_camera_matrix * tmp;

    uniform->set_value(direction,"direction");
    uniform->set_value(projection_camera_matrix,"matrix");
}

void DirectionalLight::set_uniform(UniformBlock *uniform) {
    Light::set_uniform(uniform);
    set_direction(direction);
}

/*** SpotLight ***/

SpotLight::SpotLight(UniformBlock *uniform,Vec3<float> pos,Vec3<float> direction,float intensity,Vec3<float> color,float illu_angle,float max_illu_angle) : 
    PointLight(uniform,pos,intensity,color) 
{
    type=SPOT_LIGHT;
    set_direction(direction);
    set_spot(illu_angle,max_illu_angle);
    uniform->set_value(type,"light_type");
}

void SpotLight::set_spot(float illu_angle,float max_illu_angle) {
    if(illu_angle<max_illu_angle) {
        std::cout<<"Illumination angle is less than maximum illumination angle!"<<std::endl;
    }
    this->illu_angle=cos(illu_angle*M_PI/180);
    this->max_illu_angle=cos(max_illu_angle*M_PI/180);
    Vec3<float> values(linear_dissipation,this->illu_angle,this->max_illu_angle);
    uniform->set_value(values,"spot_values");
    uniform->set_value(SPOT_LIGHT,"light_type");
}

void SpotLight::set_direction(Vec3<float> direction) {
    this->direction=direction;
    uniform->set_value(direction,"direction");
}

void SpotLight::set_uniform(UniformBlock *uniform) {
    PointLight::set_uniform(uniform);
    set_direction(direction);
    set_spot(illu_angle,max_illu_angle);
}


