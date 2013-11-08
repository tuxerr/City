#ifndef DEF_VBO
#define DEF_VBO

#include <iostream>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <string.h>

class VBO {
public:
    VBO(GLenum target,GLenum mode,GLenum element_type);
    ~VBO();
    void create();
    bool iscreated();
    void destroy();
    void bind();
    void unbind();
    GLuint id();
    int size();
    int element_size();
    GLenum element_type();
    void update(void *data,int size);
    void print_contents();

private:
    GLuint vbo;
    GLenum vbo_type;
    GLenum access_mode;
    GLsizei vbo_size;
    GLsizei vbo_capacity;
    GLint vbo_element_size;
    GLenum vbo_element_type;
    bool created;
};

#endif
