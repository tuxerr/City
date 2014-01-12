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
    sce.set_perspective(100,0.1,1000);
    
    Vec3<float> position(0.5,0.5,0.5),up_vector(0,0,1);
    Vec3<float> target(5,5,1);
    sce.set_camera(position,target,up_vector);

    Timer timer(50);
    Terrain terrain(&noise);

    terrain.scale(60,60,60);

    ObjFile spaceship("data/spaceship.obj");
    Object *o=sce.new_object();
    spaceship.load_in_object(o);
    o->set_draw_mode(OBJECT_DRAW_LINES);
    o->scale(0.4,0.4,0.4);
    spaceship.close();
    o->translate(0,0,0.5);
    
    Spaceship ship(o);

    //Terrain_Data tdata = terrain.generate_terrain();

    //Object *terrain_patches = sce.new_object();
    //terrain.generate_patches(400,terrain_patches);
    //terrain_patches->set_enable_draw(true);
    //terrain_patches->set_draw_mode(OBJECT_DRAW_TRIANGLES);

    DirectionalLight *l1=sce.new_directionallight(Vec3<float>(1,1,-1));
    l1->enable_shadows(false);
    l1->set_shadow_range(-1,314);
    l1->set_color(Vec3<float>(1,1,1));

    PointLight *l2=sce.new_pointlight(Vec3<float>(0,0,1),Vec3<float>(1,1,1),1);
    //PointLight *l4=sce.new_pointlight(Vec3<float>(0,0.8,1),Vec3<float>(1,1,1),1);

    for(int i=0;i<8;i++) {
        for(int j=0;j<8;j++) {
            //PointLight *l3 = sce.new_pointlight(Vec3<float>(i,j,0.8),Vec3<float>(1,1,1),1);
        }
    }
    
    Object *ground = sce.new_object();
    float vert[] = {-1, -1, 0,
        1, -1, 0,
        1, 1, 0,
        -1, 1, 0};
    float normal[] = {0,0,1,
        0,0,1,
        0,0,1,
        0,0,1};
    
    unsigned int index[] = {0, 1, 2, 2, 3, 0};
    
    float color[] = {1, 1, 1,
        1, 0, 1,
        0, 1, 1,
        1, 1, 0};
    
    ground->update_vertices_buffer(&vert[0],sizeof(vert));
    ground->update_triangles_index_buffer(&index[0],sizeof(index));
    ground->update_color_buffer(&color[0],sizeof(color));
    ground->update_normals_buffer(&normal[0],sizeof(normal));
    
    ground->set_draw_mode(OBJECT_DRAW_TRIANGLES);
    ground->scale(2,2,2);
    ground->set_enable_draw(true);
    
    Object* gridmatrix = sce.new_object();
    float gridmatrix_vert[] = {
        0, 0, 1,
        3, 0, 1,
        0, 1, 1,
        3, 1, 1,
        0, 2, 1,
        3, 2, 1,
        0, 3, 1,
        3, 3, 1,
    };
    float gridmatrix_normal[] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    };
    float gridmatrix_color[] = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
    };
    unsigned int gridmatrix_index[] = {
        0,1,
        2,3,
        4,5,
        6,7,
        0,3,
        5,6,
        6,2,
    };
    
    gridmatrix->update_vertices_buffer(&gridmatrix_vert[0],sizeof(gridmatrix_vert));
    gridmatrix->update_lines_index_buffer(&gridmatrix_index[0],sizeof(gridmatrix_index));
    gridmatrix->update_color_buffer(&gridmatrix_color[0],sizeof(gridmatrix_color));
    gridmatrix->update_normals_buffer(&gridmatrix_normal[0],sizeof(gridmatrix_normal));

    gridmatrix->set_draw_mode(OBJECT_DRAW_LINES);
    gridmatrix->scale(2,2,1);
    gridmatrix->set_enable_draw(true);
    
    //o->translate(20,0,1);
    
    float view_distance=1500;
    
    sce.display_texture(Scene::DT_NONE);
    float posx=0,posz=1;
    
    while (!c.quit_program()) {
        disp.new_draw();

        ship.camera_config(position,target,up_vector);
       // sce.set_camera(position,target,up_vector);


//        ship.move(c.is_pressed(CT_UP),c.is_pressed(CT_DOWN), c.is_pressed(CT_RIGHT), c.is_pressed(CT_LEFT));
        if(c.is_pressed(CT_DOWN)) {
     //       l1->enable_shadows(false);
        } else if(c.is_pressed(CT_UP)) {
     //       l1->enable_shadows(true);
        }

        if(c.is_pressed(CT_LEFT)) {
            //o->rotate(-2,0,0,1);
            posx-=0.05;

        }
        if(c.is_pressed(CT_RIGHT)) {
            //o->rotate(2,0,0,1);
            posx+=0.05;

        }
        l2->set_pos(Vec3<float>(posx,0,1));
        if(c.is_pressed(CT_UP)) {
            posz+=0.05;
            gridmatrix->scale(0.9,0.9,0.9);
        } else if(c.is_pressed(CT_DOWN)) {
            posz-=0.05;
            gridmatrix->scale(1.1,1.1,1.1);
        }

        if(c.is_pressed('d')) {
            view_distance-=100;
            sce.set_perspective(90,1,view_distance);
        }

        //if(c.e) {
        //    view_distance+=100;
        //    sce.set_perspective(90,1,view_distance);
        //}

        sce.render();
        //disp.new_draw();
        //sce.draw_object(o);
       
        disp.refresh();
        c.refresh();

        timer.wait();
    }
}
