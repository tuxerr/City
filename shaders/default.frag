#version 330

layout(row_major) uniform;

in vec3 color;

void main(void) {
     gl_FragColor=vec4(color,1.0);
}