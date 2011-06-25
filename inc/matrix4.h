#ifndef DEF_MATRIX4
#define DEF_MATRIX4

#include <iostream>
#include <vector>
#include <math.h>
#include "vec3.h"

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
    Matrix4 operator*(const Matrix4 &mat);
    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float angle,float x, float y, float z);

    std::vector<float> val;
};

#endif
