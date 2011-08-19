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
    return 0;
}

int main(int argc,char *argv[]) {
    Display disp(1024,1024,false,true);     disp.init();     
    disp.new_program("shaders/default.vert","shaders/default.frag");
    disp.new_program("shaders/phong.vert","shaders/phong.frag","phong");
    disp.new_program("shaders/depth_creation.vert","shaders/depth_creation.frag","depth_creation");

    UniformBlock *matrices=disp.new_uniformblock("GlobalValues");
    disp.link_program_to_uniformblock("default",matrices);
    disp.link_program_to_uniformblock("phong",matrices);
    disp.link_program_to_uniformblock("depth_creation",matrices);

    Scene sce(&disp,matrices);
    
    Vec3<float> camerapos(-30,-30,3.5);
    sce.set_camera(camerapos,Vec3<float>(-5,-5,2),Vec3<float>(0,0,1));
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
    o->set_draw_mode(OBJECT_DRAW_LINES);
    o->translate(-5,-5,2);

    float terrain_detail=5;
    for(int i=-40;i<40;i+=terrain_detail) {
        for(int j=-40;j<40;j+=terrain_detail) {
            Object *t=sce.new_object();
            terrain.generate_terrain(Vec2<float>(-terrain_detail/2,-terrain_detail/2),terrain_detail,terrain_detail,t);
            t->set_draw_mode(OBJECT_DRAW_LINES);
            t->set_program("phong");
            t->translate(i+terrain_detail/2,j+terrain_detail/2,0); 
        }
    }

    spaceship.close();

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(0,-2,-2),Vec3<float>(1,0,1));
    l1->enable_shadows(true);
    
    int i=0;
    timer.init();
    while(!c.quit) {
        i++;
        sce.render();

        camerapos=camerapos+Vec3<float>(0.03,0.03,0);
        sce.set_camera(camerapos,camerapos+Vec3<float>(10,10,-3),Vec3<float>(0,0,1));

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
