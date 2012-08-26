#ifndef DEF_SCENE
#define DEF_SCENE

#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <math.h>
#include "display.hpp"
#include "object.hpp"
#include "light.hpp"
#include "vec3.hpp"
#include "constants.hpp"
#include "fbo.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "octree.hpp"
#include "frustum.hpp"
#include "logger.hpp"
#include "display.hpp"

#define DIRECTIONAL_LIGHT_HEIGHT 600

class Scene {
public:
    typedef enum Display_Texture {
        DT_NONE, DT_DEPTH, DT_TEST, DT_CASCADED1, DT_CASCADED2, DT_CASCADED3, DT_CASCADED4, 
        DT_CASCADED5, DT_CASCADED6, DT_CASCADED7, DT_CASCADED8, DT_NORMAL
    } Display_Texture;

    Scene(Display *disp);
    ~Scene();
    void set_perspective(float angle,float near,float far);    
    void set_perspective_ortho(float width,float near,float far);
    void set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>up_vector);
    Object* new_object();
    void delete_object(Object *o);
    void render();
    void render_directional_shadowmap(DirectionalLight* dirlight,FBO &fbo,Uniform *shadowmap_uni);
    void draw_scene(std::string program_name = "");
    void draw_octree(Octree &oct,bool testcollision,std::list<Object*> &drawn,std::string program_name);
    void draw_object(Object *o,bool use_shaders);
    PointLight* new_pointlight(Vec3<float> pos,Vec3<float> color=Vec3<float>(1,1,1),float intensity=1);
    SpotLight* new_spotlight(Vec3<float> pos,Vec3<float> color,Vec3<float> direction,float illu_angle,float max_illu_angle,float intensity=1);
    DirectionalLight* new_directionallight(Vec3<float> direction,Vec3<float> color=Vec3<float>(1,1,1),float intensity=1);
    void delete_light(Light* l);
    void display_texture(Display_Texture tex);

private:
    Vec3<float> calculate_shadowing_optimal_point(Vec3<float> near_values[4],Vec3<float> far_values[4],float &radius);

    std::set<Object*> objects;
    Light* lights[MAX_LIGHTS];
    int light_number;
    Display *disp;
    UniformBlock *globalvalues;
    UniformBlock *uniform_lights[MAX_LIGHTS];
    Uniform *uniform_light_sampler[MAX_LIGHTS];
    Uniform *uniform_light_samplercube[MAX_LIGHTS];
    Uniform *uniform_light_number;
    Uniform *uniform_cascaded_shading_zdelta;

    Uniform *uniform_displaytex_tex;
    Uniform *uniform_displaytex_arraytex;
    Uniform *uniform_displaytex_choice;

    Object* fullscreen_quad;
    Display_Texture displayed_texture;

    Texture *testtex;

    UniformBlock *uniform_light_projection;
    Vec3<float> camera_pos;
    Vec3<float> eye_vector;
    Vec3<float> up_vector;
    Matrix4 camera;
    Matrix4 perspective;
    bool camera_changed;

    float scene_near;
    float scene_far;
    float scene_fov_rad;

    Octree octree;
    Frustum frustum;
};

#endif
