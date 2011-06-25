#ifndef DEF_SHADER_PROGRAM
#define DEF_SHADER_PROGRAM

#include <iostream>
#include <map>
#include <string>
#include <GL/glew.h>
#include <stdio.h>
#include <string.h>
#include "matrix4.h"
#include "uniform.h"

enum Shader_Attribs {
    SHADER_VERTEX_ATTRIB, SHADER_COLOR_ATTRIB, SHADER_TEXTURE_ATTRIB
};

class Program {
public:
    ~Program();
    void load_shaders(const char *vertex_shader_path,const char *fragment_shader_path);
    GLuint compile_shader(const char *path,GLenum shader_type);
    void subscribe_to_uniform(Uniform *uni);
    void use();
    void unuse();
    GLuint id();
    
private:
    GLuint program_id;
    std::map<Uniform*,bool> uniforms;
};

#endif
