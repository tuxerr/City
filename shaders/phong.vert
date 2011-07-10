#version 330

layout(std140) uniform;

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

uniform GlobalMatrices {
    mat4 projection;
    mat4 modelview;
    mat4 camera;
};

out vec3 color;
smooth out pos;
smooth out normal;

void main() {
    pos=modelview*vec4(in_Vertex,1.0);
    normal=in_Normal;
    color=in_Color;

    gl_Position = projection*camera*pos;
}
