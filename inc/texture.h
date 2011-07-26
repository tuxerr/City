#ifndef DEF_TEXTURE
#define DEF_TEXTURE

#include <iostream>
#include <GL/glew.h>

#define LIGHT_SHADOWMAP_HEIGHT 1024
#define LIGHT_SHADOWMAP_WIDTH 1024

typedef enum Texture_Types {
    TEXTURE_DEPTH, TEXTURE_RGBA
} Texture_Types;

class Texture {
public:
    Texture(int width,int height,Texture_Types type);
    int get_width();
    int get_height();
    GLuint id();
    void bind(int texture_index);
    void unbind();

private:
    GLuint texture_id;
    int width;
    int height;
    Texture_Types texture_type;
};

#endif
