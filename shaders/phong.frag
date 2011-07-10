#version 140

smooth in pos;
smooth in normal;
in vec3 color;

uniform Light {
   vec3 origin;
   vec3 color;
   vec3 direction;
   float intensity;
   float linear_dissipation;
   float max_illu_angle;
   float illu_angle;
} lights[16];

uniform int light_number;

void main(void) {
     gl_FragColor=vec4(color,1.0);
}