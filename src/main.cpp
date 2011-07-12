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

    UniformBlock *matrices=disp.new_uniformblock("GlobalMatrices",sizeof(Matrix4)*4);
    disp.link_program_to_uniformblock("default",matrices);
    disp.link_program_to_uniformblock("phong",matrices);

    Scene sce(&disp,matrices);
    
    Vec3<float> camerapos(-4,2,2);
    sce.set_camera(camerapos,Vec3<float>(0,0,0),Vec3<float>(0,0,1));
    sce.set_perspective(70,1,100);

    Timer timer;
    Controls c;
    
    Object *o1=sce.new_object();
    ObjFile spaceship("data/spaceship.obj");
    spaceship.load_in_object(o1);
    o1->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o1->set_program("phong");
    o1->translate(0,0,1);
    o1->set_enable_draw(false);
    
    float v2[]= { -1, -1, 0, 
                  -1, 1, 0,
                  1, 1, 0,
                  1, -1, 0};
    float c2[]= { 1, 0, 0,
                  1, 0, 0,
                  1, 0, 0,
                  1, 0, 0 };

    float n2[]= { 0, 0, 1,
                  0, 0, 1,
                  0, 0, 1,
                  0, 0, 1 };
    
    Object *o2=sce.new_object();
    o2->set_program("phong");
    o2->set_draw_mode(OBJECT_DRAW_QUADS);
    o2->update_vertices_buffer(v2,sizeof(v2));
    o2->update_color_buffer(c2,sizeof(c2));
    o2->update_normals_buffer(n2,sizeof(n2));
    o2->scale(10,10,1);

    Light *l1=sce.new_light(Vec3<float>(1,1,1.5),Vec3<float>(0,0.3,1));

    int i=0;
    timer.init();
    while(!c.quit) {
        sce.new_draw();

        i++;
//        o1->rotate(0.1,0,0,1);
        camerapos=camerapos+Vec3<float>(0.03,0,0);
        sce.set_camera(camerapos,Vec3<float>(0,0,0),Vec3<float>(0,0,1));

        sce.draw_scene();

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
