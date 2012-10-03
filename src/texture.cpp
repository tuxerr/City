#include "texture.hpp"

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
        glTexImage2D(gl_texture_type,0,GL_RGBA32F,width,height,0,GL_RGBA,GL_FLOAT,NULL);
        break;

    case TEXTURE_DEPTH_LAYERED:
        gl_texture_type=GL_TEXTURE_2D_ARRAY;
        glBindTexture(gl_texture_type,texture_id);
        glTexImage3D(gl_texture_type,0,GL_DEPTH_COMPONENT24,width,height,SHADOWING_MAX_LAYERS,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
        break;

    default:
        std::cout<<"Texture spec problem"<<std::endl;
        break;
    }
    glTexParameterf(gl_texture_type, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameterf(gl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(gl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(gl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(gl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    unbind();
}

Texture::Texture(const char *filename) : width(-1), height(-1), texture_type(TEXTURE_RGBA), gl_texture_type(GL_TEXTURE_2D) {
    texture_id = SOIL_load_OGL_texture
	(
            filename,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );
    
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

void Texture::set_data(void *data,int xoffset,int yoffset,int data_width,int data_height) {
    if(data_width==-1) {
        data_width=width;
    }
    if(data_height==-1) {
        data_height=height;
    }

    switch(texture_type) {
    case TEXTURE_RGBA:
        glTexSubImage2D(GL_TEXTURE_2D,0,xoffset,yoffset,data_width,data_height,GL_RGBA,GL_FLOAT,data);
        break;
        
    default:
        Logger::log(LOG_ERROR)<<"Can't set data in a depth texture"<<std::endl;
        break;
    }
}
