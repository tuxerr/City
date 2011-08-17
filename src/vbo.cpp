#include "vbo.h"

VBO::VBO(GLenum target,GLenum mode,int element_size,GLenum element_type)  : 
    vbo_type(target), access_mode(mode), vbo_size(0), vbo_capacity(0), vbo_element_size(element_size),
    vbo_element_type(element_type), created(false)
{
}

VBO::~VBO() {
}

bool VBO::iscreated() {
    return created;
}

void VBO::create() {
    glGenBuffers(1,&vbo);
    created=true;
}

void VBO::destroy() {
    if(created) {
        glDeleteBuffers(1,&vbo);
        created=false;
    }
}

void VBO::bind() {
    if(created) {
        glBindBuffer(vbo_type,vbo);
    }
}

void VBO::unbind() {
    glBindBuffer(vbo_type,0);
}

GLuint VBO::id() {
    if(created) {
        return vbo;
    } else {
        return 0;
    }
}

int VBO::size() {
    return vbo_size;
}

int VBO::element_size() {
    return vbo_element_size;
}

GLenum VBO::element_type() {
    return vbo_element_type;
}
 
void VBO::update(void *data,int size) {
    if(!created) {
        return;
    }
    bind();
    if(size>vbo_capacity) {
        // resizing the VBO
        vbo_capacity=size;
        glBufferData(vbo_type,vbo_capacity,NULL,access_mode);
        glBufferSubData(vbo_type,0,size,data);
        vbo_size=size;
    } else {
        void *map_vbo=glMapBuffer(vbo_type,GL_WRITE_ONLY);
        if(map_vbo==NULL) {
            std::cout<<"Error while tranferring data to VBO"<<std::endl;
            vbo_size=-1;
        } else {
            memcpy(map_vbo,data,size);
            vbo_size=size;
        }
        glUnmapBuffer(vbo_type);
    }
}

void VBO::print_contents() {
    bind();
    std::cout<<"Printing VBO contents"<<std::endl;
    if(!created) {
        std::cout<<"VBO isn't allocated"<<std::endl;
        return;
    }

    if(vbo_element_type==GL_FLOAT) {
        float *map_vbo=(float*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
        for(int i=0;i<size()/4;i++) {
            std::cout<<i<<" : "<<map_vbo[i]<<std::endl;
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    } else if(vbo_element_type==GL_UNSIGNED_INT || vbo_element_type==GL_INT) {
        int *map_vbo=(int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_ONLY);
        if(map_vbo==NULL) {
            std::cout<<"VBO contents aren't accessible"<<std::endl;
        } else {
            for(int i=0;i<size()/4;i++) {
                std::cout<<i<<" : "<<map_vbo[i]<<std::endl;
            }
        }
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }
    unbind();

}
