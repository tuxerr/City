#version 330

smooth in vec4 pos;
out vec4 outcol;

void main(void) {
     float in_depth = (pos.z/pos.w)/2+0.5;
     outcol=vec4(in_depth,in_depth,in_depth,1);
}
