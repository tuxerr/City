in vec3 vertex;

void main(void) {
     gl_Position = glModelViewProjectionMatrix * vec4(vertex,1.0);
}
