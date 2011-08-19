#ifndef DEF_OBJECT
#define DEF_OBJECT

#include <iostream>
#include <map>
#include <string>
#include <list>
#include <GL/glew.h>
#include "vbo.h"
#include "matrix4.h"
#include "shader_program.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

typedef struct ObjectPart {
    VBO vbo,ibo_lines,ibo_triangles,ibo_quads,cbo,tbo,nbo;
    float lodmindist; /* for LOD purposes, minimum display distance */
    float bounding_sphere_size;
    int bounding_sphere_weight;
} ObjectPart;

typedef enum Object_Draw_Modes {
    OBJECT_DRAW_TRIANGLES = GL_TRIANGLES, OBJECT_DRAW_LINES = GL_LINES, OBJECT_DRAW_QUADS = GL_QUADS
} Object_Draw_Modes;

class Object {
public:
    Object();
    void destroy();
    int new_part();
    int new_lod(float lodmindist);
    void update_vertices_buffer(void *data,int size,unsigned int part_number=0,unsigned int lod_number=0);
    void update_normals_buffer(void *data,int size,unsigned int part_number=0,unsigned int lod_number=0);
    void update_lines_index_buffer(void *data,int size,unsigned int part_number=0,unsigned int lod_number=0);
    void update_triangles_index_buffer(void *data,int size,unsigned int part_number=0,unsigned int lod_number=0);
    void update_quads_index_buffer(void *data,int size,unsigned int part_number=0,unsigned int lod_number=0);
    void update_color_buffer(void *data,int size,unsigned int part_number=0,unsigned int lod_number=0);
    void update_texture_buffer(void *data,int size,unsigned int part_number=0,unsigned int lod_number=0);
    float bounding_size();
    bool need_to_update_matrices();
    void update_matrices(Matrix4 *perspective,Matrix4 *camera);
    void enable_color(bool color);
    void enable_textures(bool textures);
    void set_enable_draw(bool draw);
    bool enable_draw();
    void set_draw_mode(Object_Draw_Modes draw_mode);
    Matrix4 &modelview_matrix();
    Matrix4 &projection_modelview_matrix();
    Matrix4 &normal_matrix();
    std::string get_program();
    void set_program(std::string name);
    void draw(float distance_from_camera);
    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float angle,float x, float y, float z);
    Vec3<float> position();
    
private:
    std::vector< std::vector<ObjectPart> > parts; /* parts[PART][LOD] */
    bool ena_colors;
    bool ena_textures;
    bool ena_draw;
    Object_Draw_Modes obj_draw_mode;
    std::string program_name;
    Matrix4 obj_modelview;
    bool modelview_changed;
    Matrix4 total_modelview;
    Matrix4 projection_modelview;
    Matrix4 normal_mat;
    Vec3<float> pos;

    float bounding_sphere_size;
    void calculate_bounding_sphere();
};

#endif
