#include "object.h"

Object::Object() : 
    ena_colors(true),
    ena_draw(true),
    obj_draw_mode(OBJECT_DRAW_LINES),
    program_name("default"),
    modelview_changed(true)
{
    new_part();                 // create part 0.
    obj_modelview.identity();
}

void Object::destroy() {
    // destroy all subparts VBOs
    for(std::vector<ObjectPart>::iterator it=parts.begin();it!=parts.end();it++) {
        it->vbo.destroy();
        it->ibo_lines.destroy();
        it->ibo_triangles.destroy();
        it->ibo_quads.destroy();
        it->cbo.destroy();
        it->tbo.destroy();
    }
}

int Object::new_part() {
    ObjectPart part = 
    {
        VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_FLOAT),
        VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,2,GL_UNSIGNED_INT),
        VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_UNSIGNED_INT),
        VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,4,GL_UNSIGNED_INT),
        VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_FLOAT),
        VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_FLOAT),
        VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,3,GL_FLOAT) 
    };

    parts.push_back(part);
    return parts.size()-1;
}

void Object::update_vertices_buffer(void *data,int size,unsigned int part_number) {
    if(part_number<parts.size()) {
        if(!parts[part_number].vbo.iscreated())
            parts[part_number].vbo.create();
        parts[part_number].vbo.update(data,size);
    }
}

void Object::update_normals_buffer(void *data,int size,unsigned int part_number) {
    if(part_number<parts.size()) {
        if(!parts[part_number].nbo.iscreated())
            parts[part_number].nbo.create();
        parts[part_number].nbo.update(data,size);
    }
}

void Object::vertices_buffer_link(unsigned int src_part,unsigned int dest_part) {
    if(src_part<parts.size() && dest_part<parts.size()) {
        parts[dest_part].vbo=parts[src_part].vbo;
    }
}

void Object::update_lines_index_buffer(void *data,int size,unsigned int part_number) {
    if(part_number<parts.size()) {
        if(!parts[part_number].ibo_lines.iscreated())
            parts[part_number].ibo_lines.create();
        parts[part_number].ibo_lines.update(data,size);
    }
}

void Object::update_triangles_index_buffer(void *data,int size,unsigned int part_number) {
    if(part_number<parts.size()) {
        if(!parts[part_number].ibo_triangles.iscreated())
            parts[part_number].ibo_triangles.create();
        parts[part_number].ibo_triangles.update(data,size);
    }
}

void Object::update_quads_index_buffer(void *data,int size,unsigned int part_number) {
    if(part_number<parts.size()) {
        if(!parts[part_number].ibo_quads.iscreated())
            parts[part_number].ibo_quads.create();
        parts[part_number].ibo_quads.update(data,size);
    }
}

void Object::update_color_buffer(void *data,int size,unsigned int part_number) {
    if(part_number<parts.size()) {
        if(!parts[part_number].cbo.iscreated())
            parts[part_number].cbo.create();
        parts[part_number].cbo.update(data,size);
    }
}

void Object::update_texture_buffer(void *data,int size,unsigned int part_number) {
    if(part_number<parts.size()) {
        if(!parts[part_number].tbo.iscreated())
            parts[part_number].tbo.create();
        parts[part_number].tbo.update(data,size);
    }
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

void Object::set_draw_mode(Object_Draw_Modes draw_mode) {
    obj_draw_mode=draw_mode;
}

Matrix4 &Object::modelview_matrix() {
    return obj_modelview;
}

Matrix4 &Object::projection_modelview_matrix() {
    return projection_modelview;
}

Matrix4 &Object::normal_matrix() {
    return normal_mat;
}

std::string Object::get_program() {
    return program_name;
}

void Object::set_program(std::string name) {
    program_name=name;
}

void Object::draw() {
    for(std::vector<ObjectPart>::iterator it=parts.begin();it!=parts.end();it++) {
        
        it->vbo.bind();
        glEnableVertexAttribArray(SHADER_VERTEX_ATTRIB);
        glVertexAttribPointer(SHADER_VERTEX_ATTRIB,it->vbo.element_size(),it->vbo.element_type(),GL_FALSE,0,0);

        if(ena_colors && it->cbo.size()>0) {
            it->cbo.bind();
            glEnableVertexAttribArray(SHADER_COLOR_ATTRIB);
            glVertexAttribPointer(SHADER_COLOR_ATTRIB,it->cbo.element_size(),it->cbo.element_type(),GL_FALSE,0,0);
        }

        if(it->nbo.size()>0) {
            it->nbo.bind();
            glEnableVertexAttribArray(SHADER_NORMAL_ATTRIB);
            glVertexAttribPointer(SHADER_NORMAL_ATTRIB,it->nbo.element_size(),it->nbo.element_type(),GL_FALSE,0,0);
        }

        if(obj_draw_mode==OBJECT_DRAW_LINES && it->ibo_lines.size()>0) {
            it->ibo_lines.bind();
            glDrawElements(GL_LINES,it->ibo_lines.size()/it->ibo_lines.element_size(),it->ibo_lines.element_type(),0);

        } else if(obj_draw_mode==OBJECT_DRAW_QUADS && it->ibo_quads.size()>0) {
            it->ibo_quads.bind();
            glDrawElements(GL_QUADS,it->ibo_quads.size()/it->ibo_quads.element_size(),it->ibo_quads.element_type(),0);

        } else if(obj_draw_mode==OBJECT_DRAW_TRIANGLES && it->ibo_triangles.size()>0) {
            it->ibo_triangles.bind();
            glDrawElements(GL_TRIANGLES,it->ibo_triangles.size()/it->ibo_triangles.element_size(),it->ibo_triangles.element_type(),0);

        } else {
            // draw using standard index if nothing else has been found
            if(it->vbo.element_type()==GL_FLOAT) {
                glDrawArrays(obj_draw_mode,0,it->vbo.size()/(it->vbo.element_size()*sizeof(float)));
            } else if(it->vbo.element_type()==GL_DOUBLE) {
                glDrawArrays(obj_draw_mode,0,it->vbo.size()/(it->vbo.element_size()*sizeof(double)));
            }
        }
    
        if(ena_colors && it->cbo.size()>0) {
            glDisableVertexAttribArray(SHADER_COLOR_ATTRIB);
        }
        glDisableVertexAttribArray(SHADER_VERTEX_ATTRIB);
    }
}

void Object::translate(float x, float y, float z) {
    obj_modelview.translate(x,y,z);
    pos=pos+Vec3<float>(x,y,z);
    modelview_changed=true;
}

void Object::scale(float x, float y, float z) {
    obj_modelview.scale(x,y,z);
    modelview_changed=true;
}

void Object::rotate(float angle,float x, float y, float z) {
    obj_modelview.rotate(angle,x,y,z);
    modelview_changed=true;
}

bool Object::need_to_update_matrices() {
    return modelview_changed;
}

void Object::update_matrices(Matrix4 *perspective,Matrix4 *camera) {
    if(camera!=NULL) {
        total_modelview = (*camera)*obj_modelview;

        normal_mat = obj_modelview; 
        normal_mat.invert();
        normal_mat.transpose();

        if(perspective!=NULL) {
            projection_modelview = (*perspective)*total_modelview;
        }
    }
    modelview_changed=false;
}

Vec3<float> Object::position() {
    return pos;
}
