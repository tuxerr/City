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

void main() {
    gl_Position = projection*camera*modelview*vec4(in_Vertex, 1.0);
    color=in_Color;
}
