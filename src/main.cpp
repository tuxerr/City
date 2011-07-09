#include <iostream>
#include <string>
#include <GL/glew.h>
#include "display.h"
#include "timer.h"
#include "controls.h"
#include "building.h"
#include "scene.h"
#include "uniform.h"
#include "uniformblock.h"
#include "objfile.h"
#include "matrix4.h"

using namespace std;

int main(int argc,char *argv[]) {
    Display disp(1024,768,false);     disp.init();     
    disp.new_program("shaders/default.vert","shaders/default.frag");

    UniformBlock *matrices=disp.new_uniformblock("Global",sizeof(Matrix4)*3);
    Uniform *camera=disp.new_uniform("camera",UNIFORM_MAT4);
    Uniform *projection=disp.new_uniform("projection",UNIFORM_MAT4);
    Uniform *modelview=disp.new_uniform("modelview",UNIFORM_MAT4);
    disp.link_program_to_uniformblock("default",matrices);
    disp.link_program_to_uniform("default",camera);
    disp.link_program_to_uniform("default",projection);
    disp.link_program_to_uniform("default",modelview);

    Scene sce(&disp,matrices,camera,modelview);
    sce.set_camera(Vec3<float>(2,2,2),Vec3<float>(0,0,0),Vec3<float>(0,0,1));

    disp.perspective(70,1,100,matrices,projection);

    Timer timer;
    Controls c;
    
    Object *o1=sce.new_object();

    ObjFile spaceship("data/spaceship.obj");
    spaceship.load_in_object(o1);
    o1->set_draw_mode(GL_TRIANGLES);

    timer.init();
    while(!c.quit) {
        sce.new_draw();

        o1->rotate(0.2,0,0,1);

        sce.draw_scene();

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
