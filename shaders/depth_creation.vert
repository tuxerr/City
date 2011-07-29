#version 330

layout(row_major) uniform;

in vec3 in_Vertex;
in vec3 in_Color;

out vec3 color;
smooth out vec4 pos;

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    vec3 camera_pos;
} GlobalValues;

uniform mat4 light_projection; //perspective*camera*modelview

void main() {
    gl_Position = light_projection*GlobalValues.modelview*vec4(in_Vertex,1.0);
    pos = gl_Position;
    color = in_Color;
}
