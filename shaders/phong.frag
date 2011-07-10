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
} Lights[8];

uniform int lightnumber;

vec4 spotlight(int lightID) {
     vec4 globalcolor = (vec4(color,1.0)+vec4(Lights[lightID].color,1.0))*0.5;
     vec4 point_to_light = (vec4(Lights[lightID].origin,1.0)-pos);
     vec4 norm_normal = normalize(normal);

     float diffuse_mult_factor = dot(normalize(point_to_light),norm_normal);
     float specular_mult_factor = abs(normalize(dot(normalize(-pos),reflect(point_to_light,norm_normal))));

     vec4 diffuse = diffuse_mult_factor*globalcolor*0.35;
     vec4 ambiant = globalcolor*0.25;
     vec4 specular = pow(specular_mult_factor,10)*globalcolor*0.2;

    return ambiant+diffuse+specular;
}

void main(void) {
     vec4 res = vec4(0,0,0,0);
     for(int i=0;i<lightnumber;i++) {
             res+=spotlight(i);
     }
     gl_FragColor = res;
}       