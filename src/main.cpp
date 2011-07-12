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
    sce.set_perspective(70,1,100);

    Timer timer;
    Controls c;
    
    Object *o1=sce.new_object();
    ObjFile spaceship("data/spaceship.obj");
    spaceship.load_in_object(o1);
    o1->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o1->set_program("phong");
    
    Light *l1=sce.new_light(Vec3<float>(2,0,1),Vec3<float>(0,0,1));
    Light *l2=sce.new_light(Vec3<float>(0,2,1),Vec3<float>(1,0,0));

    int i=0;
    timer.init();
    while(!c.quit) {
        sce.new_draw();

        i++;
        o1->rotate(0.5,0,0,1);

        sce.draw_scene();
        if(i==100) {
            sce.set_camera(Vec3<float>(1,1,2),Vec3<float>(0,0,0),Vec3<float>(0,0,1));
        }

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
