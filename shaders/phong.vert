#version 330

layout(std140) uniform;

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

out vec3 color;
smooth out vec4 pos;
smooth out vec4 normal;

uniform GlobalMatrices {
    mat4 projection;
    mat4 modelview;
    mat4 camera;
};

void main() {
    pos=modelview*vec4(in_Vertex,1.0);
    normal=vec4(in_Normal,1.0);
    color=in_Color;

    gl_Position = projection*camera*pos;
}
