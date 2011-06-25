#version 140

in vec3 color;

void main(void) {
     gl_FragColor=vec4(color,1.0);
}