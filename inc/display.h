#ifndef DEF_DISPLAY
#define DEF_DISPLAY

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include "shader_program.h"
#include "uniform.h"
#include "matrix4.h"
#include "object.h"
#include "scene.h"

#define TITLE "City"

class Display{
public:
    Display(int width=800,int height=600,bool fullscreen=false);
    ~Display();
    void init();
    void perspective(float angle,float near,float far,Uniform *perspective);
    void new_program(const char *vertex_shader_path,const char *fragment_shader_path,std::string name="default");
    Uniform* new_uniform(std::string uniform_name,Uniform_Type type);
    void link_program_to_uniform(std::string program_name,Uniform *uni);
    void new_draw();
    void refresh();
    void set_modelview_uniform(Uniform *uni);
    void draw_scene(Scene &sc);

private:
    void draw_object(Object &o);

    int width;
    int height;
    bool fullscreen;
    std::map<std::string, Program> programs;
    std::vector<Uniform*> uniforms;
    Matrix4 projection;
    Uniform *modelview;
};

#endif
