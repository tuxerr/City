#version 330

in vec3 in_Vertex;
in vec3 in_Color;

out vec3 color;

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    vec3 camera_pos;
} GlobalValues;

uniform Light_properties_ {
    mat4 matrix;
} Light_properties;

void main() {
    gl_Position = Light_properties.matrix*GlobalValues.modelview*vec4(in_Vertex,1.0);
    color = in_Color;
}
