#include <iostream>
#include <string>
#include <GL/glew.h>
#include "display.h"
#include "timer.h"
#include "controls.h"
#include "building.h"
#include "scene.h"
#include "uniform.h"

using namespace std;

int main(int argc,char *argv[]) {
    Display disp(1024,768,false);     disp.init();     
    // loading default program
    disp.new_program("shaders/default.vert","shaders/default.frag");

    // loading red program
    disp.new_program("shaders/default.vert","shaders/red.frag","redfrag");

    Uniform *modelview=disp.new_uniform("modelview",UNIFORM_MAT4);
    Uniform *camera=disp.new_uniform("camera",UNIFORM_MAT4);
    Uniform *projection=disp.new_uniform("projection",UNIFORM_MAT4);
    Uniform *color=disp.new_uniform("colorlul",UNIFORM_FLOAT3);

    disp.link_program_to_uniform("default",modelview);
    disp.link_program_to_uniform("default",camera);
    disp.link_program_to_uniform("default",projection);
    disp.link_program_to_uniform("redfrag",modelview);
    disp.link_program_to_uniform("redfrag",camera);
    disp.link_program_to_uniform("redfrag",projection);
    disp.link_program_to_uniform("redfrag",color);
    disp.set_modelview_uniform(modelview);



    Scene sce(camera);
    sce.set_camera(Vec3<float>(2,2,2),Vec3<float>(0,0,0),Vec3<float>(0,0,1));

    disp.perspective(70,1,100,projection);

    Timer timer;
    Controls c;
    
    int v1_id=sce.new_object();
    Object *o1=sce.get_object(v1_id);

    int v2_id=sce.new_object();
    Object *o2=sce.get_object(v2_id);

    int v3_id=sce.new_object();
    Object *o3=sce.get_object(v3_id);

    float v1[] = {0,0,0, 1,0,0};
    float c1[] = {1.0, 0.0, 0.0, 1,0,0};

    float v2[] = {0,0,0, 0,1,0};
    float c2[] = {0.0, 1.0, 0.0, 0,1,0};

    float v3[] = {0,0,0, 0,0,1};
    float c3[] = {1,1,1,1,1,1};

    o1->update_vertices_buffer(v1,sizeof(v1));
    o1->update_color_buffer(c1,sizeof(c1));
	o1->set_program("redfrag");

    o2->update_vertices_buffer(v2,sizeof(v2));
    o2->update_color_buffer(c2,sizeof(c2));
    o2->set_program("redfrag");

    o3->update_vertices_buffer(v3,sizeof(v3));
    o3->update_color_buffer(c3,sizeof(c3));
    o3->set_program("redfrag");

    timer.init();
    Vec3<float> color_val(1.0,1.0,1.0);
    while(!c.quit) {
        color_val=color_val+Vec3<float>(0.01,0.01,0.01);
        if(color_val.norm()>1) {
            color_val=Vec3<float>(0,0,0);
        }
        
        disp.new_draw();

        color->set_value(color_val);

        disp.draw_scene(sce);
        c.refresh();
        disp.refresh();
        timer.wait();
    }
}
