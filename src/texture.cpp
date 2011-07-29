#include "texture.h"

Texture::Texture(int width,int height,Texture_Types type) : width(width), height(height), texture_type(type) {
    glGenTextures(1,&texture_id);
    glBindTexture(GL_TEXTURE_2D,texture_id);
    switch(type) {
    case TEXTURE_DEPTH:
        glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width,height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        break;
        
    case TEXTURE_RGBA:
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_FLOAT,NULL);
        break;

    default:
        std::cout<<"Texture spec problem"<<std::endl;
        break;
    }
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D,0);
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
    glBindTexture(GL_TEXTURE_2D,texture_id);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D,0);
}
