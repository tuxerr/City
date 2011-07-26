#ifndef DEF_SCENE
#define DEF_SCENE

#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include "display.h"
#include "object.h"
#include "light.h"
#include "vec3.h"
#include "constants.h"

class Scene {
public:
    Scene(Display *disp,UniformBlock *matrices);
    ~Scene();
    void new_draw();
    void set_perspective(float angle,float near,float far);    
    void set_perspective_ortho(float width,float near,float far);
    void set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>up_vector);
    Object* new_object();
    void delete_object(Object *o);
    void draw_scene(std::string program_name = "");
    void draw_object(Object *o,bool use_shaders);
    PointLight* new_pointlight(Vec3<float> pos,Vec3<float> color,float intensity=1);
    SpotLight* new_spotlight(Vec3<float> pos,Vec3<float> color,Vec3<float> direction,float illu_angle,float max_illu_angle,float intensity=1);
    DirectionalLight* new_directionallight(Vec3<float> direction,Vec3<float> color,float intensity=1);
    void delete_light(Light* l);

private:
    std::set<Object*> objects;
    Light* lights[MAX_LIGHTS];
    int light_number;
    Display *disp;
    UniformBlock *matrices;
    UniformBlock *uniform_lights[MAX_LIGHTS];
    Uniform *uniform_light_number;
    Vec3<float> camera_pos;
    Matrix4 camera;
    Matrix4 perspective;
    bool camera_changed;
    bool perspective_changed;
};

#endif
