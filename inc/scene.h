#ifndef DEF_SCENE
#define DEF_SCENE

#include <iostream>
#include <set>
#include <string>
#include "display.h"
#include "object.h"
#include "light.h"
#include "vec3.h"

class Scene {
public:
    Scene(Display *disp,UniformBlock *matrices);
    ~Scene();
    void new_draw();
    void set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>axis);
    Object* new_object();
    void delete_object(Object *o);
    void draw_scene();
    void draw_object(Object *o);
    Light *new_light(Vec3<float> pos,Vec3<float> color=Vec3<float>(1,1,1) ,float intensity=1);
    void delete_light(Light* l);

private:
    std::set<Object*> objects;
    std::set<Light*> lights;
    Display *disp;
    UniformBlock *matrices;
};

#endif
