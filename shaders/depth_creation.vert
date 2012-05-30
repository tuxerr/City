#version 330

in vec3 in_Vertex;

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    float near;
    vec3 camera_pos;
    vec3 eye_vector;
    float far;
} GlobalValues;

uniform Light_properties_ {
    mat4 matrix;
} Light_properties;

void main() {
    gl_Position = Light_properties.matrix*GlobalValues.modelview*vec4(in_Vertex,1.0);   
}
