#version 410

in vec3 in_Vertex;

uniform sampler2D tex;
uniform sampler2DArray arraytex;
uniform int choice; //choice == -1 : use tex as texture. else use choice as arraytex layer.

void main() {
     gl_Position = vec4(in_Vertex.xy,0.,1.0);
}
