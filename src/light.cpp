#include "light.h"

Light::Light(Vec3<float> pos,float intensity,Vec3<float> color) : pos(pos), color(color), intensity(intensity) {
}

void Light::set_pos(Vec3<float> pos) {
    this.pos=pos;
}

void Light::set_color(Vec3<float> color) {
    this.color=color;
}

void Light::set_intensity(Vec3<float> intensity) {
    this.intensity=intensity;
}
