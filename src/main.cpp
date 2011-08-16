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
#include "fbo.h"
#include "terrain.h"
#include "math.h"

using namespace std;

float terrainheight(float x, float y) {
    return sin(x*y);
}

int main(int argc,char *argv[]) {
    Display disp(1024,1024,false);     disp.init();     
    disp.new_program("shaders/default.vert","shaders/default.frag");
    disp.new_program("shaders/phong.vert","shaders/phong.frag","phong");
    disp.new_program("shaders/depth_creation.vert","shaders/depth_creation.frag","depth_creation");

    UniformBlock *matrices=disp.new_uniformblock("GlobalValues");
    disp.link_program_to_uniformblock("default",matrices);
    disp.link_program_to_uniformblock("phong",matrices);
    disp.link_program_to_uniformblock("depth_creation",matrices);

    Scene sce(&disp,matrices);
    
    Vec3<float> camerapos(5,5,4);
    sce.set_camera(camerapos,Vec3<float>(0,0,1),Vec3<float>(0,0,1));
    sce.set_perspective(FOV,1,100);

    Timer timer;
    Terrain terrain(&terrainheight,0.2);
    
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
    o1->scale(10,10,10);
    o1->set_enable_draw(false);

    ObjFile spaceship("data/spaceship.obj");

    Object *o=sce.new_object();
    o->set_program("phong");
    spaceship.load_in_object(o);
    o->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o->translate(-5,-5,2);

    Object *t=sce.new_object();
    terrain.generate_terrain(Vec2<float>(-10,-10),20,20,t);
    t->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    t->set_program("phong");

    spaceship.close();

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(0,0,-1),Vec3<float>(1,1,1));
//    PointLight *l=sce.new_pointlight(Vec3<float>(0,0,5),Vec3<float>(1,0,0));
    
    int i=0;
    timer.init();
    while(!c.quit) {
        i++;
        sce.render();

        o->translate(0.02,0.02,0);
        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
