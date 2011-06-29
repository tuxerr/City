#include "scene.h"

Scene::Scene(Display *disp,Uniform *camera,Uniform *modelview) : disp(disp), camera(camera), modelview(modelview) {
}

Scene::~Scene() {
    std::set<Object*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++) {
        delete_object(*it);
    }
    objects.clear();
}

void Scene::new_draw() {
    disp->new_draw();
}

void Scene::set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>axis) {
    Matrix4 cam;
    cam.clear();

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

    camera->set_value(cam);
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

        modelview->set_value(o->modelview_matrix());

        program->use();

        o->draw();

        program->unuse();
    }
}
