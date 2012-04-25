#include "light.hpp"

Light::Light(UniformBlock *uniform,float intensity,Vec3<float> color) : 
    uniform(uniform), type(OFF), color(color), intensity(intensity), linear_dissipation(1), 
    shadowmap(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE,TEXTURE_DEPTH_LAYERED),render_shadows(false),
    shadow_min_range(-1), shadow_max_range(-1)
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

void Light::enable_shadows(bool shadow) {
    render_shadows=shadow;
    uniform->set_value(render_shadows,"render_shadows");
}

void Light::set_shadow_range(float min,float max) {
    shadow_min_range=min;
    shadow_max_range=max;
    uniform->set_value(shadow_min_range,"shadow_min_range");
    uniform->set_value(shadow_max_range,"shadow_max_range");
}

float Light::get_shadow_min_range() {
    return shadow_min_range;
}

float Light::get_shadow_max_range() {
    return shadow_max_range;
}

bool Light::enable_shadows() {
    return render_shadows;
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
    set_shadow_range(shadow_min_range,shadow_max_range);
    enable_shadows(render_shadows);
}

Texture* Light::get_depth_texture() {
    return &shadowmap;
}

UniformBlock* Light::get_uniformblock() {
    return uniform;
}

Light_Types Light::get_type() {
    return type;
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
    enable_shadows(true);
    uniform->set_value(type,"light_type");
}

void DirectionalLight::set_direction(Vec3<float> direction) {
    this->direction=direction;
    uniform->set_value(direction,"direction");
}

void DirectionalLight::set_uniform(UniformBlock *uniform) {
    Light::set_uniform(uniform);
    set_direction(direction);
}

Vec3<float> DirectionalLight::get_direction() {
    return direction;
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


