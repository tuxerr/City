#include "object.hpp"
#include "octree.hpp"

Object::Object(Octree *tree) : 
    has_been_drawn(false),
    ena_colors(true),
    ena_textures(true),
    ena_draw(true),
    has_normals(false),
    obj_draw_mode(OBJECT_DRAW_LINES),
    program_name("deferred"),
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
        GLuint vao;
        glGenVertexArrays(1, &vao);
        ObjectPart part = 
        {
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_VERTEX_ATTRIB), // vbo
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo lines
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo triangles
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo quads
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_COLOR_ATTRIB), // cbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_TEXTURE_ATTRIB), // tbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_NORMAL_ATTRIB), // nbo
            vao,
            OBJECT_DRAW_NONE,
            -1,                          // minimum viewing distance for the LOD
            0,
            0
        };
        v.push_back(part);

    } else {
        for(unsigned int i=0;i<parts[0].size();i++) { //creating N parts if we currently have N LODs for part 0
            GLuint vao;
            glGenVertexArrays(1, &vao);
            ObjectPart part = 
            {
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_VERTEX_ATTRIB), // vbo
                VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo lines
                VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo triangles
                VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo quads
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_COLOR_ATTRIB), // cbo
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_TEXTURE_ATTRIB), // tbo
                VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_NORMAL_ATTRIB), // nbo
                vao,
                OBJECT_DRAW_NONE,
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
        GLuint vao;
        glGenVertexArrays(1, &vao);
        ObjectPart part =
        {
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_VERTEX_ATTRIB), // vbo
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo lines
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo triangles
            VBO(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,GL_UNSIGNED_INT,vao), // ibo quads
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_COLOR_ATTRIB), // cbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_TEXTURE_ATTRIB), // tbo
            VBO(GL_ARRAY_BUFFER,GL_STATIC_DRAW,GL_FLOAT,vao,SHADER_NORMAL_ATTRIB), // nbo
            vao,
            OBJECT_DRAW_NONE,
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

    if(lod_number==0 && tree!=NULL) {
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
    has_normals=true;
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
    for(unsigned int i=0;i<parts.size();i++) {
        for(unsigned int j=0;j<parts[i].size();j++) {
            switch(draw_mode) {
                case OBJECT_DRAW_LINES:
                    glBindVertexArray(parts[i][j].vao);
                    parts[i][j].ibo_lines.bind();
                    glBindVertexArray(0);
                    break;
                    
                case OBJECT_DRAW_TRIANGLES:
                    glBindVertexArray(parts[i][j].vao);
                    parts[i][j].ibo_triangles.bind();
                    glBindVertexArray(0);
                    break;
                    
                case OBJECT_DRAW_PATCHES:
                    glBindVertexArray(parts[i][j].vao);
                    parts[i][j].ibo_quads.bind();
                    glBindVertexArray(0);
                    break;
            }
        }
    }
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
        glBindVertexArray(parts[i][lod].vao);

        if(obj_draw_mode==OBJECT_DRAW_LINES && parts[i][lod].ibo_lines.size()>0) {
            glDrawElements(GL_LINES,parts[i][lod].ibo_lines.size()/parts[i][lod].ibo_lines.element_size(),parts[i][lod].ibo_lines.element_type(),0);
        } else if(obj_draw_mode==OBJECT_DRAW_PATCHES && parts[i][lod].ibo_quads.size()>0) {
            // patches and quads share the same VBOs, since GL_PATCH_VERTICES is 4 (patches are just special quads)
            glDrawElements(GL_PATCHES,parts[i][lod].ibo_quads.size()/parts[i][lod].ibo_quads.element_size(),parts[i][lod].ibo_quads.element_type(),0);
        } else if(obj_draw_mode==OBJECT_DRAW_TRIANGLES && parts[i][lod].ibo_triangles.size()>0) {
            parts[i][lod].ibo_triangles.bind();
            glDrawElements(GL_TRIANGLES,parts[i][lod].ibo_triangles.size()/parts[i][lod].ibo_triangles.element_size(),parts[i][lod].ibo_triangles.element_type(),0);
        }
        
        glBindVertexArray(0);
    }

}

void Object::translate(float x, float y, float z) {
    obj_modelview.translate(x,y,z);
    modelview_changed=true;
    if(tree!=NULL) {
        tree->delete_object(this);
        tree->add_object(this);
    }
}

void Object::set_pos(Vec3<float> pos) {
    set_pos(pos.x,pos.y,pos.z);
}

void Object::set_pos(float x, float y, float z) {
    Vec3<float> pos = position();
    Vec3<float> destination = Vec3<float>(x,y,z);
    Vec3<float> translation = destination-pos;
    translate(translation.x,translation.y,translation.z);
}

void Object::scale(float x, float y, float z) {
    obj_modelview.scale(x,y,z);
    modelview_changed=true;
    bounding_scale_factor*=maxf(maxf(x,y),z);
    if(tree!=NULL) {
        tree->delete_object(this);
        tree->add_object(this);
    }
}

void Object::reset_modelview() {
    obj_modelview.identity();
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

        if(modelview_changed && has_normals) {
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
