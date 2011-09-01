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

float terrainheight(float x, float y) {
    return 0;
}

int main(int argc,char *argv[]) {

    PerlinNoise noise(1,0.6,3,2,42);
    Display disp(1024,1024,false,true);     disp.init();     
    disp.new_program("shaders/default.vert","shaders/default.frag");
    disp.new_program("shaders/phong.vert","shaders/phong.frag","phong");
    disp.new_program("shaders/depth_creation.vert","shaders/depth_creation.frag","depth_creation");

    UniformBlock *matrices=disp.new_uniformblock("GlobalValues");
    disp.link_program_to_uniformblock("default",matrices);
    disp.link_program_to_uniformblock("phong",matrices);
    disp.link_program_to_uniformblock("depth_creation",matrices);

    Scene sce(&disp,matrices);
    
    Vec3<float> camerapos(0,0,3.5);
    sce.set_camera(camerapos,Vec3<float>(300,300,1),Vec3<float>(0,0,1));
    sce.set_perspective(FOV,1,100);

    Timer timer;
    Terrain terrain(terrainheight,0.1,&noise);
    
    Controls c;

    ObjFile spaceship("data/spaceship.obj");

    Object *o=sce.new_object();
    o->set_program("phong");
    spaceship.load_in_object(o);
    o->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o->translate(2,1,1.9);

    float terrain_detail=5;
    for(int i=0;i<150;i+=terrain_detail) {
        for(int j=0;j<150;j+=terrain_detail) {
            Object *t=sce.new_object();
            terrain.generate_terrain(Vec2<float>(i,j),terrain_detail,terrain_detail,t);
            t->set_draw_mode(OBJECT_DRAW_TRIANGLES);
            t->set_program("phong");
            t->translate(i+terrain_detail/2,j+terrain_detail/2,0); 
        }
    }

    spaceship.close();

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(2,2,-0.4),Vec3<float>(1,1,1));
    l1->enable_shadows(true);

//    PointLight *l2=sce.new_pointlight(Vec3<float>(0,0,60),Vec3<float>(1,1,1),1);
    
    int i=0;
    timer.init();
    while(!c.quit) {
        i++;
        sce.render();

        camerapos=camerapos+Vec3<float>(0.06,0.06,0);
        sce.set_camera(camerapos,camerapos+Vec3<float>(10,10,-3),Vec3<float>(0,0,1));

        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
