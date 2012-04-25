#version 330

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

out vec3 color;
smooth out vec4 vert_pos;
smooth out vec4 vert_normal;

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    vec3 camera_pos;
    vec3 eye_vector;
    float near;
    float far;
} GlobalValues;

void main() {
    vert_pos=GlobalValues.modelview*vec4(in_Vertex,1.0);
    vert_normal=GlobalValues.normal_matrix*vec4(in_Normal,1.0);
    color=in_Color;

    gl_Position = GlobalValues.projection_modelview*vec4(in_Vertex,1.0);
}
