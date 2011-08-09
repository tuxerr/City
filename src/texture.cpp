#include "texture.h"

Texture::Texture(int width,int height,Texture_Types type) : width(width), height(height), texture_type(type), gl_texture_type(GL_TEXTURE_2D) {
    glGenTextures(1,&texture_id);

    switch(type) {
    case TEXTURE_DEPTH:
        gl_texture_type=GL_TEXTURE_2D;
        glBindTexture(gl_texture_type,texture_id);
        glTexImage2D(gl_texture_type,0,GL_DEPTH_COMPONENT24,width,height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
        break;
        
    case TEXTURE_RGBA:
        gl_texture_type=GL_TEXTURE_2D;
        glBindTexture(gl_texture_type,texture_id);
        glTexImage2D(gl_texture_type,0,GL_RGBA,width,height,0,GL_RGBA,GL_FLOAT,NULL);
        break;

    case TEXTURE_DEPTH_LAYERED:
        gl_texture_type=GL_TEXTURE_2D_ARRAY;
        glBindTexture(gl_texture_type,texture_id);
        glTexImage3D(gl_texture_type,0,GL_DEPTH_COMPONENT24,width,height,CASCADED_SHADING_DEPTH,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
        break;

    default:
        std::cout<<"Texture spec problem"<<std::endl;
        break;
    }
    glTexParameterf(gl_texture_type, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameterf(gl_texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(gl_texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(gl_texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(gl_texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(gl_texture_type,0);
}

Texture::~Texture() {
    glDeleteTextures(1,&texture_id);
}

GLuint Texture::id() {
    return texture_id;
}

int Texture::get_width() {
    return width;
}

int Texture::get_height() {
    return height;
}

void Texture::bind(int texture_index) {
    if(texture_index!=-1) {
        glActiveTexture(GL_TEXTURE0 + texture_index);
    }
    glBindTexture(gl_texture_type,texture_id);
}

void Texture::unbind() {
    glBindTexture(gl_texture_type,0);
}

GLenum Texture::get_gl_texture_type() {
    return gl_texture_type;
}
