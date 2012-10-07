#version 330

#define FOG_DENSITY 0.001

// outputs to 3 GL_RGBA32F textures 
layout(location = 0) out vec4 normalmap;
layout(location = 1) out vec4 colormap;
layout(location = 2) out vec4 texturemap;

in vec3 color;
smooth in vec4 vert_normal;
smooth in vec3 texcoord;

uniform sampler2D terrain_geo;
uniform sampler2D terrain_color;
uniform float terrain_height;
uniform float terrain_length;
uniform float lod_factor;
uniform vec2 screen_size;

void main(void) {
    normalmap=vert_normal;
    colormap = vec4(color,1.);
    texturemap = vec4(texcoord.xyz,1);
    //vert_pos will be recreated via the depth buffer
}
