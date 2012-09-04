#version 330

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

out vec3 color;
smooth out vec4 vert_normal;
smooth out vec3 texcoord;

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    float near;
    vec3 camera_pos;
    vec3 eye_vector;
    float far;
} GlobalValues;

void main() {
    vert_normal=GlobalValues.normal_matrix*vec4(in_Normal,1.0);
    color=in_Color;
    texcoord=in_TexCoord;
    gl_Position = GlobalValues.projection_modelview*vec4(in_Vertex,1.0);
}
