#include "scene.h"

Scene::Scene(Display *disp,UniformBlock *matrices) : disp(disp), matrices(matrices) {
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
    Light *l=new Light(pos,intensity,color);
    lights.insert(l);
    return l;
}

void Scene::delete_light(Light* l) {
    if(l!=NULL) {
        delete l;
    }
    lights.erase(l);
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
