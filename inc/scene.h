#ifndef DEF_SCENE
#define DEF_SCENE

#include <iostream>
#include <set>
#include <string>
#include "display.h"
#include "object.h"
#include "vec3.h"

class Scene {
public:
    Scene(Display *disp,Uniform *camera,Uniform *modelview);
    ~Scene();
    void new_draw();
    void set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>axis);
    Object* new_object();
    void delete_object(Object *o);
    void draw_scene();
    void draw_object(Object *o);

private:
    std::set<Object*> objects;
    Display *disp;
    Uniform *camera;
    Uniform *modelview;
};

#endif
