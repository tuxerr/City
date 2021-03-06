#ifndef DEF_SCENE
#define DEF_SCENE

#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <math.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
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
        DT_CASCADED5, DT_CASCADED6, DT_CASCADED7, DT_CASCADED8, DT_NORMAL, DT_COLOR, DT_TEXCOORD
    } Display_Texture;

    Scene(Display *disp);
    ~Scene();
    void set_perspective(float angle,float near,float far);    
    void set_perspective_ortho(float width,float near,float far);
    void set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>up_vector);
    Object* new_object();
    Object* new_object_outside_octree();
    void delete_object(Object *o);
    void render();
    void render_directional_shadowmap(DirectionalLight* dirlight);
    void draw_scene();
    void draw_octree(Octree &oct,bool testcollision,std::list<Object*> &drawn);
    void draw_object(Object *o);
    PointLight* new_pointlight(Vec3<float> pos,Vec3<float> color=Vec3<float>(1,1,1),float intensity=1);
    SpotLight* new_spotlight(Vec3<float> pos,Vec3<float> color,Vec3<float> direction,float illu_angle,float max_illu_angle,float intensity=1);
    DirectionalLight* new_directionallight(Vec3<float> direction,Vec3<float> color=Vec3<float>(1,1,1),float intensity=1);
    void delete_light(Light* l);
    void display_texture(Display_Texture tex);
    Matrix4 camera;
    Matrix4 perspective;


private:
    Vec3<float> calculate_shadowing_optimal_point(Vec3<float> near_values[4],Vec3<float> far_values[4],float &radius);

    std::set<Object*> objects;
    Light* lights[MAX_LIGHTS];
    int light_number;
    Display *disp;
    UniformBlock *globalvalues;
    UniformBlock *uniform_light;
    Uniform *uniform_light_sampler;
    Uniform *uniform_light_samplercube;
    Uniform *uniform_cascaded_shading_zdelta;

    Uniform *uniform_displaytex_tex;
    Uniform *uniform_displaytex_arraytex;
    Uniform *uniform_displaytex_choice;

    Uniform *uniform_phong_normalmap;
    Uniform *uniform_phong_depthmap;
    Uniform *uniform_phong_colormap;
    Uniform *uniform_phong_texcoordmap;
    Uniform *uniform_postprocess_colormap;
    
    Display_Texture displayed_texture;

    Texture *null_colortex;
    Texture *deferred_normalmap;
    Texture *deferred_colormap;
    Texture *deferred_texcoordmap;
    Texture *deferred_depthmap;
    Texture *deferred_result;
    Texture *postprocess_flip1;
    Texture *postprocess_flip2;
    
    FBO fbo_shadows,fbo_deferred,fbo_deferred_phong,fbo_postprocess;

    UniformBlock *uniform_light_projection;
    Vec3<float> camera_pos;
    Vec3<float> eye_vector;
    Vec3<float> up_vector;
    
    Object* cube;
    void generate_cube();
    Object* fullscreen_quad;
    void generate_fullscreen_quad();
    void set_deferred_cube_position(PointLight *l);

    bool camera_changed;

    float scene_near;
    float scene_far;
    float scene_fov_rad;

    Octree octree;
    Frustum frustum;
};

#endif
