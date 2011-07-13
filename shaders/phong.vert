#version 330

layout(std140,row_major) uniform;

in vec3 in_Vertex;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_TexCoord;

out vec3 color;
smooth out vec4 vert_pos;
smooth out vec4 vert_normal;

uniform GlobalValues {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    vec3 camera_pos;
};

void main() {
    vert_pos=modelview*vec4(in_Vertex,1.0);
    vert_normal=normal_matrix*vec4(in_Normal,1.0);
    color=in_Color;

    gl_Position = projection_modelview*vec4(in_Vertex,1.0);
}
