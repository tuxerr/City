#ifndef DEF_DISPLAY
#define DEF_DISPLAY

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include "shader_program.h"
#include "uniform.h"
#include "uniformblock.h"
#include "matrix4.h"

#define TITLE "City"

class Display{
public:
    Display(int width=800,int height=600,bool fullscreen=false);
    ~Display();
    void init();
    void perspective(float angle,float near,float far,UniformBlock *matrices,Uniform *projection);
    void new_program(const char *vertex_shader_path,const char *fragment_shader_path,std::string name="default");
    Program *get_program(std::string name);
    Uniform* new_uniform(std::string uniform_name,Uniform_Type type);
    UniformBlock* new_uniformblock(std::string uniform_name,int size);
    void link_program_to_uniform(std::string program_name,Uniform *uni);
    void link_program_to_uniformblock(std::string program_name,UniformBlock *uni);
    void new_draw();
    void refresh();

private:
    SDL_WindowID screen;
    SDL_GLContext glcontext;
    int width;
    int height;
    bool fullscreen;
    std::map<std::string, Program> programs;
    std::set<Uniform*> uniforms;
    std::set<UniformBlock*> uniformblocks;
    Matrix4 projection;
};

#endif
