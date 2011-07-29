#version 330

out vec4 outcol;
in vec3 color;

void main(void) {
    outcol = vec4(color,1.0);
}
