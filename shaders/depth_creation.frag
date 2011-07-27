#version 330

layout(row_major) uniform;

in vec3 color;
out vec4 outcol;

void main(void) {
     outcol=vec4(color,1.0);
}