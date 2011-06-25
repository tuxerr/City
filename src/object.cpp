#include "object.h"

Object::Object() : 
    vbo(GL_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_FLOAT),
    ibo_lines(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,2,GL_UNSIGNED_INT), 
    ibo_triangles(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_UNSIGNED_INT), 
    ibo_quads(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,4,GL_UNSIGNED_INT),
    cbo(GL_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_FLOAT),
    ena_colors(true),
    ena_draw(true),
    obj_draw_mode(GL_LINES),
    program_name("default")
{
    modelview.identity();
}


void Object::update_vertices_buffer(void *data,int size) {
    vbo.update(data,size);
}

void Object::update_lines_index_buffer(void *data,int size) {
    ibo_lines.update(data,size);
}

void Object::update_triangles_index_buffer(void *data,int size) {
    ibo_triangles.update(data,size);
}

void Object::update_quads_index_buffer(void *data,int size) {
    ibo_quads.update(data,size);
}

void Object::update_color_buffer(void *data,int size) {
    cbo.update(data,size);
}

void Object::enable_color(bool color) {
    ena_colors=color;
}

void Object::set_enable_draw(bool draw) {
    ena_draw=draw;
}

bool Object::enable_draw() {
    return ena_draw;
}

void Object::set_draw_mode(GLenum draw_mode) {
    obj_draw_mode=draw_mode;
}

Matrix4 &Object::modelview_matrix() {
    return modelview;
}

std::string Object::get_program() {
    return program_name;
}

void Object::set_program(std::string name) {
    program_name=name;
}

void Object::draw() {
    vbo.bind();
    glEnableVertexAttribArray(SHADER_VERTEX_ATTRIB);
    glVertexAttribPointer(SHADER_VERTEX_ATTRIB,vbo.element_size(),vbo.element_type(),GL_FALSE,0,0);

    if(ena_colors && cbo.size()>0) {
        cbo.bind();
        glEnableVertexAttribArray(SHADER_COLOR_ATTRIB);
        glVertexAttribPointer(SHADER_COLOR_ATTRIB,cbo.element_size(),cbo.element_type(),GL_FALSE,0,0);
    }

    if(obj_draw_mode==GL_LINES && ibo_lines.size()>0) {
        ibo_lines.bind();
        glDrawElements(obj_draw_mode,ibo_lines.size()/ibo_lines.element_size(),ibo_lines.element_type(),0);

    } else if(obj_draw_mode==GL_QUADS && ibo_quads.size()>0) {
        ibo_quads.bind();
        glDrawElements(obj_draw_mode,ibo_quads.size()/ibo_quads.element_size(),ibo_quads.element_type(),0);

    } else if(obj_draw_mode==GL_TRIANGLES && ibo_triangles.size()>0) {
        ibo_triangles.bind();
        glDrawElements(obj_draw_mode,ibo_triangles.size()/ibo_triangles.element_size(),ibo_triangles.element_type(),0);

    } else {
        // draw using standard index if nothing else has been found
        if(vbo.element_type()==GL_FLOAT) {
            glDrawArrays(obj_draw_mode,0,vbo.size()/(vbo.element_size()*sizeof(float)));
        } else if(vbo.element_type()==GL_DOUBLE) {
            glDrawArrays(obj_draw_mode,0,vbo.size()/(vbo.element_size()*sizeof(double)));
        }
    }
    
    if(ena_colors && cbo.size()>0) {
        glDisableVertexAttribArray(SHADER_COLOR_ATTRIB);
    }
    glDisableVertexAttribArray(SHADER_VERTEX_ATTRIB);
}

void Object::translate(float x, float y, float z) {
    modelview.translate(x,y,z);
    pos=pos+Vec3<float>(x,y,z);
}

void Object::scale(float x, float y, float z) {
    modelview.scale(x,y,z);
}

void Object::rotate(float angle,float x, float y, float z) {
    modelview.rotate(angle,x,y,z);
}

Vec3<float> Object::position() {
    return pos;
}
