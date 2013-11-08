#version 410

in vec3 in_Vertex;

smooth out vec2 deferred_texcoord;

void main() {
     deferred_texcoord = in_Vertex.xy/2+0.5;
     gl_Position = vec4(in_Vertex.xy,0.,1.0);
}
