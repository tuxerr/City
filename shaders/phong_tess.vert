#version 410

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

out vec3 vPosition;

uniform GlobalValues {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    mat4 screen_to_world;
    float near;
    vec3 camera_pos;
    vec3 eye_vector;
    float far;
} GlobalValues;

void main() {   
    vPosition = in_Vertex;
}
