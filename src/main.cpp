#include <iostream>
#include <string>
#include <GL/glew.h>
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

    UniformBlock *matrices=disp.new_uniformblock("GlobalValues");
    disp.link_program_to_uniformblock("phong",matrices);
    disp.link_program_to_uniformblock("default",matrices);

    Scene sce(&disp,matrices);
    
    Vec3<float> camerapos(2,2,2);
    sce.set_camera(camerapos,Vec3<float>(0,0,1),Vec3<float>(0,0,1));
    sce.set_perspective(70,1,100);

    Timer timer;
    Controls c;


    float v2[]= { -10, -10, 0, 
                  -10, 10, 0,
                  10, 10, 0,
                  10, -10, 0};
    float c2[]= { 1, 0, 0,
                  1, 0, 0,
                  1, 0, 0,
                  1, 0, 0 };

    float n2[]= { 0, 0, 1,
                  0, 0, 1,
                  0, 0, 1,
                  0, 0, 1 };
    

    Object *o1=sce.new_object();
    o1->set_program("phong");
    o1->set_draw_mode(OBJECT_DRAW_QUADS);
    o1->update_vertices_buffer(v2,sizeof(v2));
    o1->update_color_buffer(c2,sizeof(c2));
    o1->update_normals_buffer(n2,sizeof(n2));

    ObjFile spaceship("data/spaceship.obj");

    Object *o=sce.new_object();
    spaceship.load_in_object(o);
    o->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o->set_program("phong");
    o->translate(0,0,1);

    spaceship.close();

    Vec3<float> lightpos(-5,2,2);
    Light *l1=sce.new_light(lightpos,Vec3<float>(1,0,0));
    l1->set_spot(Vec3<float>(0,-1,-1),60,20);
    
    int i=0;
    timer.init();
    while(!c.quit) {
        sce.new_draw();

        i++;
        lightpos=lightpos+Vec3<float>(0.03,0,0);
        l1->set_pos(lightpos);

        sce.draw_scene();

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
