#version 140

in vec3 in_Vertex;
in vec3 in_Color;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 camera;

out vec3 color;

void main() {
    gl_Position = projection*camera*modelview*vec4(in_Vertex, 1.0);
    color=in_Color;
}
