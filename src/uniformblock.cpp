#include "uniformblock.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

UniformBlock::UniformBlock(std::string name,int size,int attachpoint) : 
    name(name), size(size), iscreated(false), attachpoint(attachpoint)
{
    create();
}

void UniformBlock::create() {
    glGenBuffers(1,&ubo);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo);
    glBufferData(GL_UNIFORM_BUFFER,size,NULL,GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    iscreated=true;
}

void UniformBlock::destroy() {
    if(iscreated) {
        glDeleteBuffers(1,&ubo);
        iscreated=false;
    }
}

int UniformBlock::get_attach_point() {
    return attachpoint;
}

void UniformBlock::bind_to_attach_point() {
    glBindBuffer(GL_UNIFORM_BUFFER,ubo);
    glBindBufferRange(GL_UNIFORM_BUFFER,attachpoint,ubo,0,size);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
}

std::string UniformBlock::get_name() {
    return name;
}

GLuint UniformBlock::get_ubo() {
    return ubo;
}

void UniformBlock::set_data(void *data,int size,int offset) {
    if(iscreated) {
        glBindBuffer(GL_UNIFORM_BUFFER,ubo);
        glBufferSubData(GL_UNIFORM_BUFFER,offset,size,data);
        glBindBuffer(GL_UNIFORM_BUFFER,0);
    }
}

void UniformBlock::print_contents() {
    glBindBuffer(GL_UNIFORM_BUFFER,ubo);
    float *map_ubo = (float*)glMapBuffer(GL_UNIFORM_BUFFER,GL_READ_ONLY);
    if(map_ubo==NULL) {
        std::cout<<"UBO contents are not accessible"<<std::endl;
    } else {
        for(int i=0;i<size/4;i++) {
            std::cout<<i<<" : "<<map_ubo[i]<<std::endl;
        }
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
}
