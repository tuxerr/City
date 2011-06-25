#ifndef DEF_UNIFORM
#define DEF_UNIFORM

#include <iostream>
#include <map>
#include <string>
#include <GL/glew.h>
#include "matrix4.h"
#include "vec3.h"
#include "vec2.h"

typedef enum Uniform_Type {
    UNIFORM_INT, UNIFORM_FLOAT, UNIFORM_MAT4, UNIFORM_FLOAT3, UNIFORM_INT3, 
    UNIFORM_FLOAT2, UNIFORM_INT2
} Uniform_Type;

class Uniform {
public:
    Uniform(std::string name,Uniform_Type type);
    void add_subscriber(bool *sub,GLuint program_id);
    void set_value(int val);
    void set_value(float val);
    void set_value(Matrix4 &val);
    void set_value(Vec3<float> val);
    void set_value(Vec3<int> val);
    void set_value(Vec2<float> val);
    void set_value(Vec2<int> val);
    void send_value(GLuint program_id);
    std::string name();

private:
    void reset_bools();
    
    std::string uniform_name;
    Uniform_Type type;
    int ival;
    float fval;
    Matrix4 mval;
    Vec3<float> f3val;
    Vec3<int> i3val;
    Vec2<float> f2val;
    Vec2<int> i2val;
    std::vector<bool*> sent_uniforms;
    std::map<GLuint,GLint> uniform_locations;
};

#endif
