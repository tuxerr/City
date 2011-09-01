#ifndef DEF_UTILS
#define DEF_UTILS

#include "vec3.h"

typedef struct Frustum {
    Vec3<float> normal[5];
    Vec3<float> farpoint;
    Vec3<float> origin;
} Frustum;

float maxf(float v1, float v2);

#endif
