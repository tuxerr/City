#version 330

layout(std140,row_major) uniform;

in vec3 color;

void main(void) {
     gl_FragColor=vec4(color,1.0);
}