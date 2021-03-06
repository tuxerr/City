#ifndef DEF_UTILS
#define DEF_UTILS

#include "vec3.hpp"

Vec3<float> mixvec(Vec3<float> v1,Vec3<float> v2,float mixrate); /* v1*(1-mixrate)+mixrate*v2 */

float maxf(float v1, float v2);

float minf(float v1, float v2);

float multiple_of(float n,float mult);

#endif
