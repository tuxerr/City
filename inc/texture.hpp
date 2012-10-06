#ifndef DEF_TEXTURE
#define DEF_TEXTURE

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "SOIL.h"
#include "constants.hpp"
#include "logger.hpp"

typedef enum Texture_Types {
    TEXTURE_DEPTH, TEXTURE_RGBA, TEXTURE_DEPTH_LAYERED
} Texture_Types;

class Texture {
public:
    Texture(int width,int height,Texture_Types type);
    Texture(const char *filename);
    ~Texture();
    int get_width();
    int get_height();
    GLenum get_gl_texture_type();
    GLuint id();
    void bind(int texture_index = -1);
    void unbind();
    void set_data(void *data,int xoffset=0,int yoffset=0,int data_width=-1,int data_height=-1);

private:
    GLuint texture_id;
    int width;
    int height;
    Texture_Types texture_type;
    GLenum gl_texture_type;
};

#endif
