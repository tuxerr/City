#version 410

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

out vec3 color;

uniform GlobalValues {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    mat4 screen_to_world; //inverse(perspective*camera)
    float near;
    vec3 camera_pos;
    vec3 eye_vector;
    float far;
} GlobalValues;

void main() {
    gl_Position = GlobalValues.projection_modelview*vec4(in_Vertex,1.0);
    //gl_Position = vec4(in_Vertex,1.0);

    color=in_Color;
}
