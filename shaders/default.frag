#version 410

in vec3 color;
out vec4 outColor;

void main(void) {
     outColor=vec4(color,1.0);
}