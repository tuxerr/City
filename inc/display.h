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
#include "constants.h"

class Display{
public:
    Display(int width=800,int height=600,bool fullscreen=false,bool aa=true);
    ~Display();
    void init();
    void viewport(int new_width = -1, int new_height = -1);
    int get_width();
    int get_height();
    void new_program(const char *vertex_shader_path,const char *fragment_shader_path,std::string name="default");
    bool has_program(std::string name);
    Program *get_program(std::string name);
    Uniform* new_uniform(std::string uniform_name,Uniform_Type type);
    UniformBlock* new_uniformblock(std::string uniform_name);
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
    bool antialiasing;
    std::map<std::string, Program> programs;
    std::set<Uniform*> uniforms;
    std::set<UniformBlock*> uniformblocks;
    Matrix4 projection;
};

#endif
