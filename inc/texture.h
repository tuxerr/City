#ifndef DEF_TEXTURE
#define DEF_TEXTURE

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "constants.h"

typedef enum Texture_Types {
    TEXTURE_DEPTH, TEXTURE_RGBA, TEXTURE_DEPTH_LAYERED
} Texture_Types;

class Texture {
public:
    Texture(int width,int height,Texture_Types type);
    ~Texture();
    int get_width();
    int get_height();
    GLenum get_gl_texture_type();
    GLuint id();
    void bind(int texture_index = -1);
    void unbind();

private:
    GLuint texture_id;
    int width;
    int height;
    Texture_Types texture_type;
    GLenum gl_texture_type;
};

#endif
