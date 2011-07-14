#include "light.h"

Light::Light(UniformBlock *uniform,Vec3<float> pos,float intensity,Vec3<float> color) : 
    pos(pos), color(color), direction(Vec3<float>(0,0,0)), intensity(intensity), linear_dissipation(-1), max_illu_angle(-1), illu_angle(-1) {
    set_uniform(uniform);
}

void Light::set_pos(Vec3<float> pos) {
    this->pos=pos;
    uniform->set_value(pos,"origin");
}

void Light::set_color(Vec3<float> color) {
    this->color=color;
    uniform->set_value(color,"color");
}

void Light::set_direction(Vec3<float> direction) {
    this->direction=direction;
    uniform->set_value(direction,"direction");
}

void Light::set_intensity(float intensity) {
    this->intensity=intensity;
    uniform->set_value(intensity,"intensity");
}

void Light::set_spot(Vec3<float> direction,float illu_angle,float max_illu_angle) {
    this->direction=direction;
    set_direction(direction);
    if(illu_angle<max_illu_angle) {
        std::cout<<"Illumination angle is less than maximum illumination angle!"<<std::endl;
    }
    this->illu_angle=illu_angle;
    this->max_illu_angle=max_illu_angle;
    Vec3<float> values(linear_dissipation,illu_angle,max_illu_angle);
    uniform->set_value(values,"spot_values");
}

void Light::desactivate_spot() {
    set_spot(direction,-1,-1);
}

void Light::set_linear_dissipation(float lin_dissipation) {
    this->linear_dissipation=lin_dissipation;
    Vec3<float> values(linear_dissipation,illu_angle,max_illu_angle);
    uniform->set_value(values,"spot_values");
}

void Light::set_uniform(UniformBlock *uniform) {
    this->uniform=uniform;
    set_pos(pos);
    set_color(color);
    set_intensity(intensity);
    set_spot(direction,illu_angle,max_illu_angle);
    set_linear_dissipation(linear_dissipation);
}
