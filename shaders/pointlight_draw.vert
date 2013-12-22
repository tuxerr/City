#version 410

in vec3 in_Vertex;

uniform GlobalValues {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    mat4 screen_to_world; //inverse(perspective*camera)
    float near;
    vec3 camera_pos;
    vec3 eye_vector;
    float far;
    vec2 screen_size;
} GlobalValues;

void main() {
    vec4 point = GlobalValues.projection_modelview*vec4(in_Vertex,1.0);
    gl_Position = point;
}
