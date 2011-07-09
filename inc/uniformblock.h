#ifndef DEF_UNIFORMBLOCK
#define DEF_UNIFORMBLOCK

#include <iostream>
#include <GL/glew.h>
#include <string>

class UniformBlock {
public:
    UniformBlock(std::string name,int size,int attachpoint);
    void create();
    void destroy();
    int get_attach_point();
    void bind_to_attach_point();
    std::string get_name();
    void set_data(void *data,int size,int offset);
    GLuint get_ubo();

private:
    std::string name;
    int size;
    bool iscreated;
    int attachpoint;
    GLuint ubo;
};

#endif
