#ifndef DEF_UNIFORMBLOCK
#define DEF_UNIFORMBLOCK

#include <iostream>
#include <string>
#include <map>
#include <GL/glew.h>
#include "matrix4.hpp"
#include "vec3.hpp"
#include "vec2.hpp"

class UniformBlock {
public:
    UniformBlock(std::string name,int attachpoint);
    void create();
    void destroy();
    int get_attach_point();
    void bind_to_attach_point(GLuint program_id);
    std::string get_name();
    void set_value(bool val,std::string sub_name);
    void set_value(int val,std::string sub_name);
    void set_value(float val,std::string sub_name);
    void set_value(Vec3<float> &val,std::string sub_name);
    void set_value(Vec3<int> &val,std::string sub_name);
    void set_value(Vec2<float> &val,std::string sub_name);
    void set_value(Vec2<int> &val,std::string sub_name);
    void set_value(Matrix4 &val,std::string sub_name);
    GLuint get_ubo();
    void print_contents();

private:
    GLint get_value_from_pname(std::string sub_name,GLenum pname);
    GLint get_offset(std::string sub_name);

    std::string complete_name;
    std::string subuniform_name; /* name without the _ to query subuniforms */
    int size;
    bool iscreated;
    int attachpoint;
    GLuint ubo;
    GLuint program_id;
    GLuint ubo_loc_in_program;
    std::map<std::string,GLuint> offsets;
    std::map<std::string,GLuint> matrix_strides;
};

#endif
