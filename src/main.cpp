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
#include "octree.h"
#include "perlin.h"
#include "spaceship.h"
#include "logger.hpp"

using namespace std;

int main(int argc,char *argv[]) {

    Logger::init("city.log");

    PerlinNoise noise(0.5,2,1,4,42);
    Display disp(1680,1050,false,true);     disp.init();     
    disp.new_program("shaders/default.vert","shaders/default.frag");
    disp.new_program("shaders/phong.vert","shaders/phong.frag","phong");
    disp.new_program("shaders/depth_creation.vert","shaders/depth_creation.frag","depth_creation");

    UniformBlock *matrices=disp.new_uniformblock("GlobalValues");
    disp.link_program_to_uniformblock("default",matrices);
    disp.link_program_to_uniformblock("phong",matrices);
    disp.link_program_to_uniformblock("depth_creation",matrices);

    Scene sce(&disp,matrices);
    
    Vec3<float> camerapos(0,0,4);
    sce.set_camera(camerapos,Vec3<float>(300,300,1),Vec3<float>(0,0,1));
    sce.set_perspective(FOV,1,FAR);

    Timer timer;

    Terrain terrain(0.4,&noise);

    terrain.scale(60,60,150);
    
    Controls c;

    ObjFile spaceship("data/spaceship.obj");

    Object *o=sce.new_object();
    o->set_program("phong");
    spaceship.load_in_object(o);
    o->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o->rotate(180,0,0,1);
    o->translate(0,0,5);

    Spaceship ship(o);
    float vert[] = {-1, -1, 0,
                    1, -1, 0, 
                    1, 1, 0, 
                    -1, 1, 0};

    float norms[] = {0, 0, 1, 
                     0, 0, 1, 
                     0, 0, 1, 
                     0, 0, 1};

    int index[] = {0, 1, 2, 3};

    float color[] = {1, 1, 1,
                     1, 1, 1,
                     1, 1, 1,
                     1, 1, 1};

    float vert2[] = {0,0,0,
                     0,1,0,
                     0,1,1,
                     0,0,1};


    float norms2[] = {-1,0,0,
                      -1,0,0,
                      -1,0,0,
                      -1,0,0};

    float color2[] = {1, 0, 0,
                      1, 0, 0,
                      1, 0, 0,
                      1, 0, 0};

    Object *t = sce.new_object();
    t->set_draw_mode(OBJECT_DRAW_QUADS);
    t->set_program("phong");
    t->update_vertices_buffer(&vert[0],sizeof(vert));
    t->update_normals_buffer(&norms[0],sizeof(norms));
    t->update_quads_index_buffer(&index[0],sizeof(index));
    t->update_color_buffer(&color[0],sizeof(color));
    t->scale(100,100,100);

    Object *t2 = sce.new_object();
    t2->set_draw_mode(OBJECT_DRAW_QUADS);
    t2->set_program("phong");
    t2->update_vertices_buffer(&vert2[0],sizeof(vert2));
    t2->update_normals_buffer(&norms2[0],sizeof(norms2));
    t2->update_quads_index_buffer(&index[0],sizeof(index));
    t2->update_color_buffer(&color2[0],sizeof(color2));
    t2->scale(10,10,10);

//    t->set_enable_draw(false);
//    t2->set_enable_draw(false);

/*    float terrain_detail=10;
    for(int i=0;i<200;i+=terrain_detail) {
        for(int j=0;j<200;j+=terrain_detail) {
            Object *t=sce.new_object();
            terrain.generate_terrain(Vec2<float>(i,j),terrain_detail,terrain_detail,t);
            t->set_draw_mode(OBJECT_DRAW_TRIANGLES);
            t->set_program("phong");
            t->translate(i+terrain_detail/2,j+terrain_detail/2,0); 
        }
        }*/



    spaceship.close();

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(1,0,-1));
    l1->enable_shadows(true);

//    PointLight *l2=sce.new_pointlight(Vec3<float>(0,0,60),Vec3<float>(1,1,1),1);
    
    int i=0;
    timer.init();
    Vec3<float> position(5,5,5),up_vector(0,0,1);
    Vec3<float> direction=Vec3<float>(0,0,0)-position;
    
    o->translate(-40,-40,30);

    while(!c.quit) {
        i++;
        sce.render();

        camerapos=camerapos+Vec3<float>(0.06,0.06,0);
        ship.move(c.up,c.down,c.right,c.left);

/*        if(c.down) {
            l1->enable_shadows(false);
        } else if(c.up) {
            l1->enable_shadows(true);
        }

        if(c.right) {
            o->rotate(-2,0,0,1);
        } 
        if(c.left) {
            o->rotate(2,0,0,1);
            } */

        ship.camera_config(position,direction,up_vector);
        sce.set_camera(position,direction,up_vector);

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
