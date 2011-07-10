#include "scene.h"

Scene::Scene(Display *disp,UniformBlock *matrices) : light_number(0), disp(disp), matrices(matrices) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        lights[i]=NULL;
    }
    if(disp.has_program("phong")) {
        *lights_number=disp.new_uniform("light_number",UNIFORM_INT);
        disp.link_program_to_uniform("phong",lights_number);
        
        for(int i=0;i<MAX_LIGHTS;i++) {
            uniform_lights[i]=disp.new_uniformblock("lights["+i+"]",sizeof(Light::uniform_size()));
            disp.link_program_to_uniformblock("phong",lights[i]);
        }
    }
}

Scene::~Scene() {
    std::set<Object*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++) {
        delete_object(*it);
    }
    objects.clear();

    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]!=NULL) {
            delete lights[i];
        }
    }
}

void Scene::new_draw() {
    disp->new_draw();
}

void Scene::set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>axis) {
    Matrix4 cam;

    Vec3<float> look=direction-pos;
    Vec3<float> normal=look*axis;
    Vec3<float> new_axis=normal*look;

    normal.normalize();
    new_axis.normalize();
    look.normalize();

    cam.val[0] = normal.x;
    cam.val[1] = normal.y;
    cam.val[2] = normal.z;

    cam.val[4] = new_axis.x;
    cam.val[5] = new_axis.y;
    cam.val[6] = new_axis.z;

    cam.val[8] = -look.x;
    cam.val[9] = -look.y;
    cam.val[10] = -look.z;

    cam.val[15] = 1.0;

    cam.translate(-pos.x,-pos.y,-pos.z);

    cam.transpose();
    matrices->set_data(&cam,sizeof(cam),2*sizeof(Matrix4));
 }

Object* Scene::new_object() {
    Object *o=new Object();
    objects.insert(o);
    return o;
}

void Scene::delete_object(Object *o) {
    if(o!=NULL) {
        o->destroy();
        delete o;
    }
    objects.erase(o);
}

Light* Scene::new_light(Vec3<float> pos,Vec3<float> color,float intensity) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]==NULL) {
            Light *l=new Light(uniform_lights[i],pos,intensity,color);
            lights[i]=l;
            return l;            
        }
    }
    
    if(!created) {
        std::cout<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
        return NULL;
    }
}

void Scene::delete_light(Light* l) {
    if(l!=NULL) {
        for(int i=0;i<MAX_LIGHTS;i++) {
            if(lights[i]==l) {
                lights[i]=NULL;
            }
        }
        delete l;
        uniform_light_number--;
    }
}

void Scene::draw_scene() {
    std::set<Object*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++) {
        draw_object(*it);
    }
}

void Scene::draw_object(Object *o) {
    if(o->enable_draw()) {
        std::string program_name=o->get_program();
        // if the object's shader doesn't exist, use default one.
        Program *program=disp->get_program(program_name);

        Matrix4 cpy = o->modelview_matrix();
        cpy.transpose();
        matrices->set_data(&cpy,sizeof(cpy),sizeof(Matrix4));

        program->use();

        o->draw();

        program->unuse();
    }
}
