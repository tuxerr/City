#version 410

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

out vec3 vPosition;

uniform sampler2D terrain_geo;
uniform sampler2D terrain_color;
uniform float terrain_height;
uniform float terrain_length;
uniform float lod_factor;
uniform vec2 screen_size;

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
    float height = texture(terrain_geo,in_Vertex.xy/terrain_length).a*terrain_height;
    vPosition = vec3(in_Vertex.xy,height);
    gl_Position = GlobalValues.projection_modelview*vec4(vPosition,1.0);
}
