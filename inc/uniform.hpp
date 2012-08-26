#ifndef DEF_UNIFORM
#define DEF_UNIFORM

#include <iostream>
#include <map>
#include <string>
#include <GL/glew.h>
#include "texture.hpp"
#include "matrix4.hpp"
#include "vec3.hpp"
#include "vec2.hpp"

class Program;

typedef enum Uniform_Type {
    UNIFORM_BOOL, UNIFORM_INT, UNIFORM_FLOAT, UNIFORM_MAT4, UNIFORM_FLOAT3, 
    UNIFORM_INT3, UNIFORM_FLOAT2, UNIFORM_INT2, UNIFORM_SAMPLER
} Uniform_Type;

typedef struct Program_Status {
    bool* binded;
    bool* uniform_sended;
} Program_Status;
 
class Uniform {
public:
    Uniform(std::string name,Uniform_Type type);
    void add_subscriber(bool *uniform_sended,GLuint program_id,bool *state);
    void add_texture(Texture **tex,Program *shaderprogram,int index);
    Uniform_Type get_type();
    void set_value(bool val);
    void set_value(int val);
    void set_value(float val);
    void set_value(Matrix4 &val);
    void set_value(Vec3<float> val);
    void set_value(Vec3<int> val);
    void set_value(Vec2<float> val);
    void set_value(Vec2<int> val);
    void set_value(Texture *tex);
    void send_value(GLuint program_id);
    std::string name();

private:
    void reset_bools();
    
    std::string uniform_name;
    Uniform_Type type;
    bool bval;
    int ival;
    float fval;
    Matrix4 mval;
    Vec3<float> f3val;
    Vec3<int> i3val;
    Vec2<float> f2val;
    Vec2<int> i2val;
    std::map<GLuint,Program_Status> programs_status; /* binded/not binded */
    std::map<GLuint,GLint> uniform_locations;
    std::map<Texture**,Program*> tex_pointers;
};

#endif
