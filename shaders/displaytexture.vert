#version 330

in vec3 in_Vertex;
in vec3 in_Color;

uniform sampler2D tex;
uniform sampler2DArray arraytex;
uniform int choice; //choice == -1 : use tex as texture. else use choice as arraytex layer.

smooth out vec3 texcoord;
out vec3 color;

void main() {
     texcoord = in_Vertex/2+0.5;
     color = in_Color;
     gl_Position = vec4(in_Vertex,1.0);
}
