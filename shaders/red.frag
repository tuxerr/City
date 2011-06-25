#version 140

in vec3 color;

uniform vec3 colorlul;        

void main(void) {
     gl_FragColor=vec4(color*colorlul,1.0);
}
