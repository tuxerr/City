#include "scene.h"

Scene::Scene(Display *disp,UniformBlock *matrices) : light_number(0), disp(disp), matrices(matrices), camera_changed(false), perspective_changed(false) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        lights[i]=NULL;
    }
    if(disp->has_program("phong")) {
        uniform_light_number=disp->new_uniform("lightnumber",UNIFORM_INT);
        disp->link_program_to_uniform("phong",uniform_light_number);

        for(int i=0;i<MAX_LIGHTS;i++) {
            std::stringstream uniform_name;
            uniform_name<<"Light["<<i<<"]";
            uniform_lights[i]=disp->new_uniformblock(uniform_name.str(),Light::uniform_size());
            disp->link_program_to_uniformblock("phong",uniform_lights[i]);
        }

        uniform_light_number->set_value(light_number);
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

void Scene::set_perspective(float angle,float near,float far) {
    perspective.clear();
    
    float f = 1.0 / tan(angle * M_PI / 360); 
    perspective.val[0]=f/((float)disp->get_width()/disp->get_height());
    perspective.val[5]=f;
    perspective.val[10]=(far + near)/(near-far);
    perspective.val[11]=2*far*near/(near-far);
    perspective.val[14]=-1;

    perspective_changed=true;
}

void Scene::set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>axis) {
    camera.clear();

    Vec3<float> look=direction-pos;
    Vec3<float> normal=look*axis;
    Vec3<float> new_axis=normal*look;

    normal.normalize();
    new_axis.normalize();
    look.normalize();

    camera.val[0] = normal.x;
    camera.val[1] = normal.y;
    camera.val[2] = normal.z;

    camera.val[4] = new_axis.x;
    camera.val[5] = new_axis.y;
    camera.val[6] = new_axis.z;

    camera.val[8] = -look.x;
    camera.val[9] = -look.y;
    camera.val[10] = -look.z;

    camera.val[15] = 1.0;

    camera.translate(-pos.x,-pos.y,-pos.z);

    camera_changed=true;
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
            light_number++;
            uniform_light_number->set_value(light_number);
            return l;            
        }
    }
    
    std::cout<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
    return NULL;
}

void Scene::delete_light(Light* l) {
    if(l!=NULL) {
        for(int i=0;i<MAX_LIGHTS;i++) {
            if(lights[i]==l) {
                for(int j=i;j<MAX_LIGHTS-1;j++) {
                    lights[j]=lights[j+1];
                    if(lights[j]!=NULL) {
                        lights[j]->set_uniform(uniform_lights[j]);
                    }
                }
                lights[MAX_LIGHTS-1]=NULL;
            }
        }
        delete l;
        light_number--;
        uniform_light_number->set_value(light_number);
    }
}

void Scene::draw_scene() {
    std::set<Object*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++) {
        Object *o=*it;
        if(o->need_to_update_matrices() || perspective_changed || camera_changed) {
            o->update_matrices(&perspective,&camera);
        }
        draw_object(o);
    }

    if(perspective_changed) {
        perspective_changed=false;
    } 
    if(camera_changed) {
        camera_changed=false;
    }

}

void Scene::draw_object(Object *o) {
    if(o->enable_draw()) {
        std::string program_name=o->get_program();
        // if the object's shader doesn't exist, use default one.
        Program *program=disp->get_program(program_name);

        matrices->set_data(o->modelview_matrix().adress(),sizeof(Matrix4),0);

        matrices->set_data(o->projection_modelview_matrix().adress(),sizeof(Matrix4),sizeof(Matrix4));

        matrices->set_data(o->normal_matrix().adress(),sizeof(Matrix4),sizeof(Matrix4)*2);

        program->use();

        o->draw();

        program->unuse();
    }
}
