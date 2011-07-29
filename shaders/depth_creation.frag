#version 330

layout(row_major) uniform;

out vec4 outcol;
in vec3 color;
smooth in vec4 pos;

void main(void) {
    outcol = vec4(color,1.0);
//    gl_FragDepth = (pos.z/pos.w)/2+0.5;
}
