#include "utils.hpp"

Vec3<float> mixvec(Vec3<float> v1,Vec3<float> v2,float mixrate) {
    if(mixrate>1) {
        mixrate=1;
    } else if(mixrate<0) {
        mixrate=0;
    }

    Vec3<float> res;
    res=(v1*(1-mixrate))+v2*mixrate;
    return res;
}

float maxf(float v1, float v2) {
    if(v1>v2) {
        return v1;
    } else {
        return v2;
    }
}
