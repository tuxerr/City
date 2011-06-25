#ifndef DEF_VBO
#define DEF_VBO

#include <iostream>
#include <GL/glew.h>
#include <string.h>

class VBO {
public:
    VBO(GLenum target,GLenum mode,int element_size,GLenum element_type);
    ~VBO();
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
};

#endif
