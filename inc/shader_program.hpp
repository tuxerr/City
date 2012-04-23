#ifndef DEF_SHADER_PROGRAM
#define DEF_SHADER_PROGRAM

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <GL/glew.h>
#include <stdio.h>
#include <string.h>
#include "matrix4.hpp"
#include "uniform.hpp"
#include "uniformblock.hpp"

enum Shader_Attribs {
    SHADER_VERTEX_ATTRIB, SHADER_COLOR_ATTRIB, SHADER_TEXTURE_ATTRIB, SHADER_NORMAL_ATTRIB
};

class Program {
public:
    Program();
    ~Program();
    void load_shaders(const char *vertex_shader_path,const char *fragment_shader_path);
    GLuint compile_shader(const char *path,GLenum shader_type);
    void subscribe_to_uniform(Uniform *uni);
    void subscribe_to_uniformblock(UniformBlock *uni);
    void use();
    void unuse();
    GLuint id();
    
private:
    GLuint program_id;
    std::map<Uniform*,bool> uniforms;
    std::list<Texture*> textures;
    bool binded;
};

#endif
