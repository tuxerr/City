#include <iostream>
#include <string>
#include "display.h"
#include "timer.h"
#include "controls.h"
#include "scene.h"
#include "object.h"
#include "light.h"
#include "uniform.h"
#include "uniformblock.h"
#include "objfile.h"

using namespace std;

int main(int argc,char *argv[]) {
    Display disp(1024,768,false);     disp.init();     
    disp.new_program("shaders/default.vert","shaders/default.frag");
    disp.new_program("shaders/phong.vert","shaders/phong.frag","phong");

    UniformBlock *matrices=disp.new_uniformblock("GlobalMatrices",sizeof(Matrix4)*3);
    disp.link_program_to_uniformblock("default",matrices);
    disp.link_program_to_uniformblock("phong",matrices);

    Scene sce(&disp,matrices);
    sce.set_camera(Vec3<float>(2,2,2),Vec3<float>(0,0,0),Vec3<float>(0,0,1));

    disp.perspective(70,1,100,matrices);

    Timer timer;
    Controls c;
    
    Object *o1=sce.new_object();
    ObjFile spaceship("data/spaceship.obj");
    
    Light *l1=sce.new_light(Vec3<float>(0,2,2),Vec3<float>(1,0,1));
    l1->set_color(Vec3<float>(1,0,1));

    spaceship.load_in_object(o1);
    o1->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o1->set_program("phong");

    int i=0;
    timer.init();
    while(!c.quit) {
        sce.new_draw();

        i++;
        o1->rotate(0.2,0,0,1);

        if(i==100) {

        }

        sce.draw_scene();

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
