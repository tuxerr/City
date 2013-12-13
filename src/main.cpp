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
    
    Vec3<float> camerapos(5,0,0);
    sce.set_camera(camerapos,Vec3<float>(0,0,2),Vec3<float>(0,0,1));
    sce.set_perspective(100,0.1,1000);

    Timer timer(40);

    Terrain terrain(&noise);

    terrain.scale(60,60,60);

    ObjFile spaceship("data/spaceship.obj");

    
    Object *o=sce.new_object();
    spaceship.load_in_object(o);
    o->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    o->set_enable_draw(true);

    spaceship.close();
    
    Spaceship ship(o);

    //Terrain_Data tdata = terrain.generate_terrain();

    Object *terrain_patches = sce.new_object();
    terrain.generate_patches(400,terrain_patches);
    terrain_patches->set_enable_draw(true);
    terrain_patches->set_draw_mode(OBJECT_DRAW_TRIANGLES);

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(1,1,-1));
    l1->enable_shadows(false);
    l1->set_shadow_range(-1,314);
    l1->set_color(Vec3<float>(1,0,0));
    //l1->desactivate();
    

    PointLight *l2=sce.new_pointlight(Vec3<float>(0,0,4),Vec3<float>(0,1,0),1.0);
    //l2->desactivate();

    Vec3<float> position(5,0,2),up_vector(0,0,1);
    Vec3<float> target(0,0,0);
    
    //o->translate(20,0,1);
    
    float view_distance=1500;
    
    sce.display_texture(Scene::DT_NONE);
    float pos=0;
    
    while (!c.quit_program()) {
        disp.new_draw();

//        ship.camera_config(position,target,up_vector);

//        ship.move(c.is_pressed(CT_UP),c.is_pressed(CT_DOWN), c.is_pressed(CT_RIGHT), c.is_pressed(CT_LEFT));
        if(c.is_pressed(CT_DOWN)) {
     //       l1->enable_shadows(false);
        } else if(c.is_pressed(CT_UP)) {
     //       l1->enable_shadows(true);
        }

        if(c.is_pressed(CT_LEFT)) {
            o->rotate(-2,0,0,1);
        } 
        if(c.is_pressed(CT_RIGHT)) {
            o->rotate(2,0,0,1);
        }
        if(c.is_pressed(CT_UP)) {
            pos-=0.05;
        } else if(c.is_pressed(CT_DOWN)) {
            pos+=0.05;
        }
        l2->set_pos(Vec3<float>(0,pos,4));
        
        if(c.is_pressed(CT_UP)) {
            //l2->toggle();
        }
        if(c.is_pressed(CT_DOWN)) {
           // l1->toggle();
        }

        if(c.is_pressed('d')) {
            view_distance-=100;
            sce.set_perspective(90,1,view_distance);
        }

        //if(c.e) {
        //    view_distance+=100;
        //    sce.set_perspective(90,1,view_distance);
        //}

        sce.set_camera(position,target,up_vector);
        sce.render();
        //disp.new_draw();
        //sce.draw_object(o);
       
        disp.refresh();
        c.refresh();

        timer.wait();
    }
}
