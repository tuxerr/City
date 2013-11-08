#ifndef DEF_FBO
#define DEF_FBO

#include <iostream>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "texture.hpp"
#include <OpenGL/gl3.h>

typedef enum FBO_Type {
    FBO_DRAW, FBO_READ, FBO_BOTH
} FBO_Type;

typedef enum FBO_Attachment {
    FBO_DEPTH = GL_DEPTH_ATTACHMENT, FBO_STENCIL = GL_STENCIL_ATTACHMENT, 
    FBO_COLOR0 = GL_COLOR_ATTACHMENT0,  FBO_COLOR1 = GL_COLOR_ATTACHMENT1,
    FBO_COLOR2 = GL_COLOR_ATTACHMENT2,  FBO_COLOR3 = GL_COLOR_ATTACHMENT3,
    FBO_COLOR4 = GL_COLOR_ATTACHMENT4,  FBO_COLOR5 = GL_COLOR_ATTACHMENT5,
    FBO_COLOR6 = GL_COLOR_ATTACHMENT6,  FBO_COLOR7 = GL_COLOR_ATTACHMENT7
} FBO_Attachment;

class FBO {
public:
    FBO();
    ~FBO();
    bool iscomplete();
    void bind(FBO_Type = FBO_BOTH);
    void unbind();
    void attach_texture(Texture *tex,FBO_Attachment attachpoint,int array_layer=-1);

private:
    GLuint fbo_id;
};

#endif
