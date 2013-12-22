#include "light.hpp"

Light::Light(float intensity,Vec3<float> color) :
    type(OFF), color(color), intensity(intensity), linear_dissipation(1),
    shadowmap(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE,TEXTURE_DEPTH_LAYERED),render_shadows(false),
    shadow_min_range(-1), shadow_max_range(-1), activated(true)
{
}

float Light::get_intensity() {
    return intensity;
}

void Light::set_color(Vec3<float> color) {
    this->color=color;
}

void Light::set_intensity(float intensity) {
    this->intensity=intensity;
}

void Light::activate() {
    activated=true;
}

void Light::desactivate() {
    activated=false;
}

void Light::toggle() {
    activated=!activated;
}

bool Light::is_activated() {
    return activated;
}


void Light::enable_shadows(bool shadow) {
    render_shadows=shadow;
}

void Light::set_shadow_range(float min,float max) {
    shadow_min_range=min;
    shadow_max_range=max;
    
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
}

void Light::set_uniform(UniformBlock *uniform) {
    uniform->bind();
    uniform->set_value(type,"light_type");
    uniform->set_value(color,"color");
    uniform->set_value(intensity,"intensity");
    
    Vec3<float> values(linear_dissipation,0,0);
    uniform->set_value(values,"spot_values");
    uniform->set_value(shadow_min_range,"shadow_min_range");
    uniform->set_value(shadow_max_range,"shadow_max_range");
    uniform->set_value(render_shadows,"render_shadows");
    uniform->unbind();
}

Texture* Light::get_depth_texture() {
    return &shadowmap;
}

Light_Types Light::get_type() {
    return type;
}

/*** PointLight Class ***/
PointLight::PointLight(Vec3<float> pos,float intensity,Vec3<float> color) :
    Light(intensity,color)
{
    type=POINT_LIGHT;
    set_pos(pos);
}

void PointLight::set_pos(Vec3<float> pos) {
    this->pos=pos;
}

void PointLight::set_uniform(UniformBlock *uniform) {
    uniform->bind();
    uniform->set_value(pos,"origin");
    Light::set_uniform(uniform);
}

Vec3<float> PointLight::get_pos() {
    return pos;
}


/*** DirectionalLight ***/
DirectionalLight::DirectionalLight(Vec3<float> direction,float intensity,Vec3<float> color) :
    Light(intensity,color)
{
    type=DIRECTION_LIGHT;
    set_direction(direction);
    enable_shadows(true);
}

void DirectionalLight::set_direction(Vec3<float> direction) {
    this->direction=direction;
}

Vec3<float> DirectionalLight::get_direction() {
    return direction;
}

void DirectionalLight::set_uniform(UniformBlock *uniform) {
    uniform->bind();
    uniform->set_value(direction,"direction");
    Light::set_uniform(uniform);
}

/*** SpotLight ***/
SpotLight::SpotLight(Vec3<float> pos,Vec3<float> direction,float intensity,Vec3<float> color,float illu_angle,float max_illu_angle) :
    PointLight(pos,intensity,color)
{
    type=SPOT_LIGHT;
    set_direction(direction);
    set_spot(illu_angle,max_illu_angle);
}

void SpotLight::set_spot(float illu_angle,float max_illu_angle) {
    if(illu_angle<max_illu_angle) {
        std::cout<<"Illumination angle is less than maximum illumination angle!"<<std::endl;
    }
    this->illu_angle=cos(illu_angle*M_PI/180);
    this->max_illu_angle=cos(max_illu_angle*M_PI/180);
}

void SpotLight::set_direction(Vec3<float> direction) {
    this->direction=direction;
}

void SpotLight::set_uniform(UniformBlock *uniform) {
    uniform->bind();
    PointLight::set_uniform(uniform);
    uniform->set_value(direction,"direction");
    Vec3<float> values(linear_dissipation,this->illu_angle,this->max_illu_angle);
    uniform->set_value(values,"spot_values");
}


