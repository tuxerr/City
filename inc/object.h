#ifndef DEF_OBJECT
#define DEF_OBJECT

#include <iostream>
#include <map>
#include <string>
#include <GL/glew.h>
#include "vbo.h"
#include "matrix4.h"
#include "shader_program.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

typedef struct ObjectPart {
    VBO vbo,ibo_lines,ibo_triangles,ibo_quads,cbo,tbo;
    /* reflection stuff */
} ObjectPart;

class Object {
public:
    Object();
    int new_part();
    void update_vertices_buffer(void *data,int size,unsigned int part_number=0);
    void vertices_buffer_link(unsigned int src_part,unsigned int dest_part);
    void update_lines_index_buffer(void *data,int size,unsigned int part_number=0);
    void update_triangles_index_buffer(void *data,int size,unsigned int part_number=0);
    void update_quads_index_buffer(void *data,int size,unsigned int part_number=0);
    void update_color_buffer(void *data,int size,unsigned int part_number=0);
    void update_texture_buffer(void *data,int size,unsigned int part_number=0);
    void enable_color(bool color);
    void set_enable_draw(bool draw);
    bool enable_draw();
    void set_draw_mode(GLenum draw_mode);
    Matrix4 &modelview_matrix();
    std::string get_program();
    void set_program(std::string name);
    void draw();
    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float angle,float x, float y, float z);
    Vec3<float> position();
    
private:
    std::vector<ObjectPart> parts;
    bool ena_colors;
    bool ena_draw;
    GLenum obj_draw_mode;
    Matrix4 modelview;
    Vec3<float> pos;
    std::string program_name;
};

#endif
