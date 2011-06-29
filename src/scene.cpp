#include "scene.h"

Scene::Scene(Uniform *camera) : new_object_id(0), camera(camera) {
}

Scene::~Scene() {
    std::map<int,Object*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++) {
        it->second->destroy();
        delete it->second;
    }
    objects.clear();
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
    objects.insert(std::pair<int,Object*>(new_object_id,o));
    new_object_id++;
    return o;
}

Object* Scene::get_object(int id) {
    if(objects.find(id)!=objects.end()) {
        // id is a valid object identifier
        return (objects.find(id))->second;
    } else {
        return NULL;
    }
}

void Scene::delete_object(int id) {
    Object *obj=get_object(id);
    if(obj!=NULL) {
        delete obj;
    }
    objects.erase(id);
}
