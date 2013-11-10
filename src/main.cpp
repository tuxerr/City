#include <iostream>
#include <string>
#include <math.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "display.hpp"
#include "timer.hpp"
#include "controls.hpp"
#include "scene.hpp"
#include "object.hpp"
#include "light.hpp"
#include "uniform.hpp"
#include "uniformblock.hpp"
#include "objfile.hpp"
#include "fbo.hpp"
#include "terrain.hpp"
#include "octree.hpp"
#include "perlin.hpp"
#include "spaceship.hpp"
#include "logger.hpp"

using namespace std;

int main(int argc,char *argv[]) {

    Logger::init("city.log");

    PerlinNoise noise(0.5,2,2,8,42424237);

    Display disp(800,600,false,true);     disp.init();
    Controls c(disp.getWindow());

    Scene sce(&disp);
    
    Vec3<float> camerapos(0,0,4);
    sce.set_camera(camerapos,Vec3<float>(300,300,1),Vec3<float>(0,0,1));
    sce.set_perspective(90,1,1500);

    Timer timer(20);

    Terrain terrain(&noise);

    terrain.scale(60,60,60);

    ObjFile spaceship("data/spaceship.obj");

    Object *o=sce.new_object();
    spaceship.load_in_object(o);
    o->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o->rotate(180,0,0,1);
    o->translate(0,0,0);

    spaceship.close();
    
    //Spaceship ship(o);

//    Terrain_Data tdata = terrain.generate_terrain();

    //Object *terrain_patches = sce.new_object();
    //terrain.generate_patches(400,terrain_patches);

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(1,1,-1));
    l1->enable_shadows(false);
    //l1->set_shadow_range(-1,314);

//    PointLight *l2=sce.new_pointlight(Vec3<float>(0,0,60),Vec3<float>(1,1,1),1);
    
    int i=0;

    Vec3<float> position(5,0,0),up_vector(0,0,1);
    Vec3<float> target(0,0,0);
    bool stop=false;
    
    //o->translate(20,0,1);
    
    float view_distance=1500;
    
    sce.display_texture(Scene::DT_NONE);
    //while(!c.quit) {
    while (!c.quit_program()) {
        
        //if(!stop) {
       //     ship.move(c.up,c.down,c.right,c.left);
        //}

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

        //if(c.d) {
        //    view_distance-=100;
        //    sce.set_perspective(90,1,view_distance);
        //}

        //if(c.e) {
        //    view_distance+=100;
        //    sce.set_perspective(90,1,view_distance);
        //}

        //ship.camera_config(position,target,up_vector);
        //sce.set_camera(position,target,up_vector);
        //sce.render();

        std::cout<<"Error : "<<glGetError()<<std::endl;
        disp.new_draw();
        c.refresh();
        sce.draw_object(sce.fullscreen_quad);
        disp.refresh();



        timer.wait();
    }
}
