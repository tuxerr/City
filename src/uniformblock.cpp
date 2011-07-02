#include "uniformblock.h"

UniformBlock::UniformBlock(std::string name,int size,int attachpoint) : 
    name(name), size(size), iscreated(false), attachpoint(attachpoint)
{
}

void UniformBlock::create() {
    glGenBuffers(1,&ubo);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo);
    glBufferData(GL_UNIFORM_BUFFER,size,NULL,GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER,attachpoint,ubo,0,size);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    iscreated=true;
}

void UniformBlock::destroy() {
    if(iscreated) {
        glDeleteBuffers(1,&ubo);
    }
}

int UniformBlock::get_attach_point() {
    return attachpoint;
}

std::string UniformBlock::get_name() {
    return name;
}

void UniformBlock::set_data(void *data,int size,int offset) {
    if(iscreated) {
        glBindBuffer(GL_UNIFORM_BUFFER,ubo);
        glBufferSubData(GL_UNIFORM_BUFFER,offset,size,data);
        glBindBuffer(GL_UNIFORM_BUFFER,0);
    }
}
