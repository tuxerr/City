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

using namespace std;

int main(int argc,char *argv[]) {

    PerlinNoise noise(0.5,1,1,8,42);
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

    std::cout<<"after perspective"<<std::endl;

    Timer timer;

    Terrain terrain(0.1,&noise);

    std::cout<<"after terrain class"<<std::endl;

    terrain.scale(6,6,15);

    
    Controls c;

    ObjFile spaceship("data/spaceship.obj");

    std::cout<<"b4 obj loading"<<std::endl;
    Object *o=sce.new_object();
    o->set_program("phong");
    spaceship.load_in_object(o);
    std::cout<<"after obj loading"<<std::endl;
    o->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o->translate(2,1,1.9);

    std::cout<<"b4 terrain"<<std::endl;
    float terrain_detail=15;
    for(int i=0;i<200;i+=terrain_detail) {
        for(int j=0;j<200;j+=terrain_detail) {
            Object *t=sce.new_object();
            terrain.generate_terrain(Vec2<float>(i,j),terrain_detail,terrain_detail,t);
            t->set_draw_mode(OBJECT_DRAW_TRIANGLES);
            t->set_program("phong");
            t->translate(i+terrain_detail/2,j+terrain_detail/2,0); 
        }
    }
    std::cout<<"after terrain"<<std::endl;

    spaceship.close();

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(3,3,-1),Vec3<float>(1,1,1));
    l1->enable_shadows(true);

//    PointLight *l2=sce.new_pointlight(Vec3<float>(0,0,60),Vec3<float>(1,1,1),1);
    
    int i=0;
    timer.init();
    while(!c.quit) {
        i++;
        sce.render();

        camerapos=camerapos+Vec3<float>(0.06,0.06,0);
        sce.set_camera(camerapos,camerapos+Vec3<float>(10,10,-3),Vec3<float>(0,0,1));

/*        if(i==150) {
            l1->enable_shadows(true);
        } else if(i==400) {
            l1->enable_shadows(false);
            }*/

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
