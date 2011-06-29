#include <iostream>
#include <string>
#include <GL/glew.h>
#include "display.h"
#include "timer.h"
#include "controls.h"
#include "building.h"
#include "scene.h"
#include "uniform.h"
#include "objfile.h"

using namespace std;

int main(int argc,char *argv[]) {
    Display disp(1024,768,false);     disp.init();     
    disp.new_program("shaders/default.vert","shaders/default.frag");
    disp.new_program("shaders/default.vert","shaders/red.frag","redfrag");

    Uniform *modelview=disp.new_uniform("modelview",UNIFORM_MAT4);
    Uniform *camera=disp.new_uniform("camera",UNIFORM_MAT4);
    Uniform *projection=disp.new_uniform("projection",UNIFORM_MAT4);
    Uniform *color=disp.new_uniform("colorlul",UNIFORM_FLOAT3);

    disp.link_program_to_uniform("default",modelview);
    disp.link_program_to_uniform("default",camera);
    disp.link_program_to_uniform("default",projection);
    disp.link_program_to_uniform("redfrag",modelview);
    disp.link_program_to_uniform("redfrag",camera);
    disp.link_program_to_uniform("redfrag",projection);
    disp.link_program_to_uniform("redfrag",color);
    disp.set_modelview_uniform(modelview);

    Scene sce(camera);
    sce.set_camera(Vec3<float>(2,2,2),Vec3<float>(0,0,0),Vec3<float>(0,0,1));

    disp.perspective(70,1,100,projection);

    Timer timer;
    Controls c;
    
    Object *o1=sce.new_object();

    ObjFile spaceship("data/spaceship.obj");
    spaceship.load_in_object(o1);

    timer.init();
    Vec3<float> color_val(1.0,1.0,1.0);
    while(!c.quit) {
        color_val=color_val+Vec3<float>(0.01,0.01,0.01);
        if(color_val.norm()>1) {
            color_val=Vec3<float>(0,0,0);
        }
        
        disp.new_draw();

        color->set_value(color_val);

        o1->rotate(0.2,0,0,1);

        disp.draw_scene(sce);
        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
