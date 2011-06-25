#ifndef DEF_SCENE
#define DEF_SCENE

#include <iostream>
#include <map>
#include <string>
#include "object.h"
#include "vec3.h"

class Scene {
public:
    Scene(Uniform *camera);
    ~Scene();
    void set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>axis);
    int new_object();
    Object* get_object(int id);
    void delete_object(int id);
    std::map<int,Object*> objects;

private:
    int new_object_id;
    Uniform *camera;
};

#endif
