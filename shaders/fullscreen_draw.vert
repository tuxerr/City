#version 410

in vec3 in_Vertex;

void main() {
     gl_Position = vec4(in_Vertex.xy,0.,1.0);
}
