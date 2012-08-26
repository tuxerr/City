#version 330

in vec3 in_Vertex;

smooth out texcoord;

void main() {
     texcoord = in_Vertex/2+0.5;
     gl_Position = in_Vertex;
}