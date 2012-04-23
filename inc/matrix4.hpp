#ifndef DEF_MATRIX4
#define DEF_MATRIX4

#include <iostream>
#include <vector>
#include <math.h>
#include "vec3.hpp"

class Matrix4 {
public:
    Matrix4();
    Matrix4(float *val);
    Matrix4(const Matrix4 &mat);
    void clear();
    void identity();
    void print();
    float get_val(int i);
    void set_val(int i,float f);
    float* adress();
    void transpose();
    void invert();
    Matrix4 operator*(const Matrix4 &mat);
    Vec3<float> operator*(Vec3<float> vector);
    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float angle,float x, float y, float z);
    void perspective(float angle,float near,float far,float ratio);    
    void perspective_ortho(float width,float near,float far,float ratio);
    void camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>up_vector);

    float val[16];
};

#endif
