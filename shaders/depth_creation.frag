#version 330

out vec4 outcol;
smooth in vec4 pos;

void main(void) {
     outcol=vec4(1,1,1,1);
     gl_FragDepth = pos.z*0.5+0.5;
}
