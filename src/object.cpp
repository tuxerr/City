#include "object.hpp"
#include "octree.hpp"

Object::Object(Octree *tree) : 
    has_been_drawn(false),
    ena_colors(true),
    ena_textures(true),
    ena_draw(true),
    obj_draw_mode(OBJECT_DRAW_LINES),
    program_name("default"),
    modelview_changed(true),
    tree(tree),
    bounding_sphere_size(0),
    bounding_scale_factor(1),
    lod_to_draw(-1)
{
    new_part();                 // create part 0.
    obj_modelview.identity();
}

void Object::destroy() {
    // destroy all subparts VBOs
    for(unsigned int i=0;i<parts.size();i++) {
        for(std::vector<ObjectPart>::iterator it=parts[i].begin();it!=parts[i].end();it++) {
            it->vbo.destroy();
            it->ibo_lines.destroy();
            it->ibo_triangles.destroy();
            it->ibo_quads.destroy();
            it->cbo.destroy();
            it->tbo.destroy();
        }
    }
}

int Object::new_part() {
    std::vector<ObjectPart> v;
    if(parts.size()==0) {
        ObjectPart part = 
        {
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // vbo
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo lines
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo triangles
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo quads
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // cbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // tbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // nbo
            -1,                          // minimum viewing distance for the LOD
            0,
            0
        };
        v.push_back(part);

    } else {
        for(unsigned int i=0;i<parts[0].size();i++) {
            ObjectPart part = 
            {
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // vbo
                VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo lines
                VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo triangles
                VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo quads
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // cbo
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // tbo
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // nbo
                parts[0][i].lodmindist,                          // minimum viewing distance for the LOD
                0,
                0
            };
            v.push_back(part);
        }

    }
    parts.push_back(v);
    return parts.size()-1;
}

int Object::new_lod(float lodmindist) {
    int lod_iter=parts[0].size();
    for(unsigned int i=0;i<parts[0].size();i++) {
        if(parts[0][i].lodmindist==lodmindist) {
            Logger::log(LOG_WARNING)<<"A LOD having "<<lodmindist<<" as a minimum viewing distance already exists"<<std::endl;
        } else if(parts[0][i].lodmindist>lodmindist) {
            lod_iter=i;
            break;
        }
    }

    // for each new lod, we have to create n parts, n being the number of parts in the first LOD
    for(unsigned int i=0;i<parts.size();i++) {
        ObjectPart part = 
        {
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // vbo
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo lines
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo triangles
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT), // ibo quads
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // cbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // tbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT), // nbo
            lodmindist,                                      // minimum viewing distance for the LOD
            0,
            0
        };  
        parts[i].insert(parts[i].begin()+lod_iter,part);
    }
    return lod_iter;
}

void Object::update_vertices_buffer(void *data,int size,unsigned int part_number,unsigned int lod_number) {
    
    if(part_number<parts.size() && lod_number<parts[0].size()) {
        if(!parts[part_number][lod_number].vbo.iscreated())
            parts[part_number][lod_number].vbo.create();
        parts[part_number][lod_number].vbo.update(data,size);

        if(lod_number==0) {
            float currentmax=-1;
            float *ptr=(float*)data;

            for(int i=0;i<((size/sizeof(Vec3<float>))-2);i++) {

                Vec3<float> point(ptr[3*i],ptr[3*i+1],ptr[3*i+2]);

                if(point.norm()>currentmax) {
                    currentmax=point.norm();
                }
            }

            parts[part_number][0].bounding_sphere_weight=size/12;
            parts[part_number][0].bounding_sphere_size=currentmax;
        }

    }
    calculate_bounding_sphere();

    if(lod_number==0) {
        tree->delete_object(this);
        tree->add_object(this);
    }
}

void Object::calculate_bounding_sphere() {
    int total_weight=0;
    bounding_sphere_size=0;
    for(unsigned int i=0;i<parts.size();i++) {
        total_weight+=parts[i][0].bounding_sphere_weight;
        bounding_sphere_size+=(parts[i][0].bounding_sphere_weight*parts[i][0].bounding_sphere_size);
    }
    bounding_sphere_size/=total_weight;
}

float Object::bounding_size() {
    return bounding_sphere_size*bounding_scale_factor;
}

void Object::update_normals_buffer(void *data,int size,unsigned int part_number,unsigned int lod_number) {
    if(part_number<parts.size() && lod_number<parts[0].size()) {
        if(!parts[part_number][lod_number].nbo.iscreated())
            parts[part_number][lod_number].nbo.create();
        parts[part_number][lod_number].nbo.update(data,size);
    }
}

void Object::update_lines_index_buffer(void *data,int size,unsigned int part_number,unsigned int lod_number) {
    if(part_number<parts.size() && lod_number<parts[0].size()) {
        if(!parts[part_number][lod_number].ibo_lines.iscreated())
            parts[part_number][lod_number].ibo_lines.create();
        parts[part_number][lod_number].ibo_lines.update(data,size);
    }
}

void Object::update_triangles_index_buffer(void *data,int size,unsigned int part_number,unsigned int lod_number) {
    if(part_number<parts.size() && lod_number<parts[0].size()) {
        if(!parts[part_number][lod_number].ibo_triangles.iscreated())
            parts[part_number][lod_number].ibo_triangles.create();
        parts[part_number][lod_number].ibo_triangles.update(data,size);

    }
}

void Object::update_quads_index_buffer(void *data,int size,unsigned int part_number,unsigned int lod_number) {
    if(part_number<parts.size() && lod_number<parts[0].size()) {
        if(!parts[part_number][lod_number].ibo_quads.iscreated())
            parts[part_number][lod_number].ibo_quads.create();
        parts[part_number][lod_number].ibo_quads.update(data,size);
    }
}

void Object::update_color_buffer(void *data,int size,unsigned int part_number,unsigned int lod_number) {
    if(part_number<parts.size() && lod_number<parts[0].size()) {
        if(!parts[part_number][lod_number].cbo.iscreated())
            parts[part_number][lod_number].cbo.create();
        parts[part_number][lod_number].cbo.update(data,size);
    }
}

void Object::update_texture_buffer(void *data,int size,unsigned int part_number,unsigned int lod_number) {
    if(part_number<parts.size() && lod_number<parts[0].size()) {
        if(!parts[part_number][lod_number].tbo.iscreated())
            parts[part_number][lod_number].tbo.create();
        parts[part_number][lod_number].tbo.update(data,size);
    }
}

void Object::enable_color(bool color) {
    ena_colors=color;
}

void Object::enable_textures(bool textures) {
    ena_textures=textures;
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

void Object::reset_lod_to_draw() {
    lod_to_draw=-1;
}

void Object::draw(float distance_from_camera) {
    if(lod_to_draw==-1) {
        for(unsigned int i=0;i<parts[0].size();i++) {
            if(parts[0][i].lodmindist<=distance_from_camera) {
                lod_to_draw=i;
            } else {
                break;
            }
        }
    }         
    int lod=lod_to_draw;

    for(unsigned int i=0;i<parts.size();i++) {
        parts[i][lod].vbo.bind();
        glEnableVertexAttribArray(SHADER_VERTEX_ATTRIB);
        glVertexAttribPointer(SHADER_VERTEX_ATTRIB,3,parts[i][lod].vbo.element_type(),GL_FALSE,0,0);

        if(ena_colors && parts[i][lod].cbo.size()>0) {
            parts[i][lod].cbo.bind();
            glEnableVertexAttribArray(SHADER_COLOR_ATTRIB);
            glVertexAttribPointer(SHADER_COLOR_ATTRIB,3,parts[i][lod].cbo.element_type(),GL_FALSE,0,0);
        }

        if(parts[i][lod].nbo.size()>0) {
            parts[i][lod].nbo.bind();
            glEnableVertexAttribArray(SHADER_NORMAL_ATTRIB);
            glVertexAttribPointer(SHADER_NORMAL_ATTRIB,3,parts[i][lod].nbo.element_type(),GL_FALSE,0,0);
        }

        if(ena_textures && parts[i][lod].tbo.size()>0) {
            parts[i][lod].tbo.bind();
            glEnableVertexAttribArray(SHADER_TEXTURE_ATTRIB);
            glVertexAttribPointer(SHADER_TEXTURE_ATTRIB,4,parts[i][lod].tbo.element_type(),GL_FALSE,0,0);
        }

        if(obj_draw_mode==OBJECT_DRAW_LINES && parts[i][lod].ibo_lines.size()>0) {
            parts[i][lod].ibo_lines.bind();
            glDrawElements(GL_LINES,parts[i][lod].ibo_lines.size()/parts[i][lod].ibo_lines.element_size(),parts[i][lod].ibo_lines.element_type(),0);

        } else if(obj_draw_mode==OBJECT_DRAW_QUADS && parts[i][lod].ibo_quads.size()>0) {
            parts[i][lod].ibo_quads.bind();
            glDrawElements(GL_QUADS,parts[i][lod].ibo_quads.size()/parts[i][lod].ibo_quads.element_size(),parts[i][lod].ibo_quads.element_type(),0);

        } else if(obj_draw_mode==OBJECT_DRAW_PATCHES && parts[i][lod].ibo_quads.size()>0) {
            // patches and quads share the same VBOs, since GL_PATCH_VERTICES is 4 (patches are just special quads)
            parts[i][lod].ibo_quads.bind();
            glDrawElements(GL_PATCHES,parts[i][lod].ibo_quads.size()/parts[i][lod].ibo_quads.element_size(),parts[i][lod].ibo_quads.element_type(),0);

        } else if(obj_draw_mode==OBJECT_DRAW_TRIANGLES && parts[i][lod].ibo_triangles.size()>0) {
            parts[i][lod].ibo_triangles.bind();
            glDrawElements(GL_TRIANGLES,parts[i][lod].ibo_triangles.size()/parts[i][lod].ibo_triangles.element_size(),parts[i][lod].ibo_triangles.element_type(),0);

        } else {
            // draw using standard index if nothing else has been found
            if(parts[i][lod].vbo.element_type()==GL_FLOAT) {
                glDrawArrays(obj_draw_mode,0,parts[i][lod].vbo.size()/(parts[i][lod].vbo.element_size()*sizeof(float)));
            } else if(parts[i][lod].vbo.element_type()==GL_DOUBLE) {
                glDrawArrays(obj_draw_mode,0,parts[i][lod].vbo.size()/(parts[i][lod].vbo.element_size()*sizeof(double)));
            }
        }
    
        if(ena_colors && parts[i][lod].cbo.size()>0) {
            glDisableVertexAttribArray(SHADER_COLOR_ATTRIB);
        }
        if(parts[i][lod].nbo.size()>0) {
            glDisableVertexAttribArray(SHADER_NORMAL_ATTRIB);
        }
        if(ena_textures && parts[i][lod].tbo.size()>0) {
            glDisableVertexAttribArray(SHADER_TEXTURE_ATTRIB);
        }
        glDisableVertexAttribArray(SHADER_VERTEX_ATTRIB);
    }
}

void Object::translate(float x, float y, float z) {
    obj_modelview.translate(x,y,z);
    modelview_changed=true;
    tree->delete_object(this);
    tree->add_object(this);
}

void Object::scale(float x, float y, float z) {
    obj_modelview.scale(x,y,z);
    modelview_changed=true;
    bounding_scale_factor*=maxf(maxf(x,y),z);
    tree->delete_object(this);
    tree->add_object(this);
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

        if(modelview_changed) {
            normal_mat = obj_modelview; 
            normal_mat.invert();
            normal_mat.transpose();
        }

        if(perspective!=NULL) {
            projection_modelview = (*perspective)*total_modelview;
        }

    }
    modelview_changed=false;
}

Vec3<float> Object::position() {
    return Vec3<float>(obj_modelview.val[3],obj_modelview.val[7],obj_modelview.val[11]);
}
