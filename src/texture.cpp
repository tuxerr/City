#include "texture.h"


Texture::Texture(int width,int height,Texture_Types type) : width(width), height(height), texture_type(type) {
    glGenTextures(1,&texture_id);
    glBindTexture(GL_TEXTURE_2D,texture_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    switch(type) {
    case TEXTURE_DEPTH:
        glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32,width,height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
        break;
        
    case TEXTURE_RGBA:
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_FLOAT,NULL);
        break;
    }
    glBindTexture(GL_TEXTURE_2D,0);
}

GLuint Texture::id() {
    return texture_id;
}

void Texture::bind(int texture_index) {
    glActiveTexture(GL_TEXTURE0 + texture_index);
    glBindTexture(GL_TEXTURE_2D,texture_id);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D,0);
}
