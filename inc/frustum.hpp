#ifndef DEF_FRUSTUM
#define DEF_FRUSTUM

#include <iostream>
#include "vec3.hpp"
#include "constants.hpp"

class Frustum {
public:
    
    void perspective_frustum(Vec3<float> pos, Vec3<float> direction, Vec3<float> up_vector,float ratio);
    void orthogonal_frustum(Vec3<float> pos, Vec3<float> direction, Vec3<float> up_vector,float width,float ratio);

    Vec3<float> normal[6];
    Vec3<float> farpoint;
    Vec3<float> origin;
    Vec3<float> ortho_right_vec;
    Vec3<float> ortho_up_vec;
    bool ortho_frustum;
};

#endif
