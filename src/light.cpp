#include "light.h"

Light::Light(UniformBlock *uniform,Vec3<float> pos,float intensity,Vec3<float> color) : 
    pos(pos), color(color), direction(Vec3<float>(0,0,0)), intensity(intensity), linear_dissipation(-1), max_illu_angle(-1), illu_angle(-1) {
    set_uniform(uniform);
}

void Light::set_pos(Vec3<float> pos) {
    this->pos=pos;
    uniform->set_data(&pos,sizeof(pos),0);
}

void Light::set_color(Vec3<float> color) {
    this->color=color;
    uniform->set_data(&color,sizeof(color),sizeof(Vec3<float>));
}

void Light::set_direction(Vec3<float> direction) {
    this->direction=direction;
    uniform->set_data(&direction,sizeof(direction),sizeof(Vec3<float>)*2);
}

void Light::set_intensity(float intensity) {
    this->intensity=intensity;
    uniform->set_data(&intensity,sizeof(intensity),sizeof(Vec3<float>)*3);
}

void Light::set_spot(Vec3<float> direction,float illu_angle,float max_illu_angle) {
    this->direction=direction;
    set_direction(direction);
    if(illu_angle<max_illu_angle) {
        std::cout<<"Illumination angle is less than maximum illumination angle!"<<std::endl;
    }
    this->illu_angle=illu_angle;
    this->max_illu_angle=max_illu_angle;
    uniform->set_data(&illu_angle,sizeof(illu_angle),sizeof(Vec3<float>)*3+sizeof(float)*3);
    uniform->set_data(&max_illu_angle,sizeof(max_illu_angle),sizeof(Vec3<float>)*3+sizeof(float)*2);
}

void Light::desactivate_spot() {
    set_spot(direction,-1,-1);
}

void Light::set_linear_dissipation(float lin_dissipation) {
    this->linear_dissipation=lin_dissipation;
    uniform->set_data(&lin_dissipation,sizeof(lin_dissipation),sizeof(Vec3<float>)*3+sizeof(float));
}

void Light::set_uniform(UniformBlock *uniform) {
    this->uniform=uniform;
    set_pos(pos);
    set_color(color);
    set_intensity(intensity);
    set_spot(direction,illu_angle,max_illu_angle);
    set_linear_dissipation(linear_dissipation);
}

int Light::uniform_size() {
    return sizeof(Vec3<float>)*3+sizeof(float)*4;
}
