#version 330

layout(std140) uniform;

in vec3 color;
smooth in vec4 pos;
smooth in vec4 normal;

uniform Light {
   vec3 origin;
   vec3 color;
   vec3 direction;
   float intensity;
   float linear_dissipation;
   float max_illu_angle;
   float illu_angle;
} lights[16];

uniform int lightnumber;

void main(void) {
     if(lightnumber == 0) {
          gl_FragColor=vec4(color,1);
     } else {
          gl_FragColor=vec4(1,0,0,1);
     }
}       