#version 330

layout(std140,row_major) uniform;

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

     vec3 light_ray = Lights[lightID].origin-pos.xyz;
     vec3 norm_normal = normalize(normal.xyz);
     vec3 reflected_ray = reflect(-normalize(light_ray),norm_normal);
     vec3 eye_ray = -pos.xyz;

     float diffuse_mult_factor = dot(normalize(light_ray),norm_normal);
     float specular_mult_factor = max(dot(normalize(eye_ray),reflected_ray),0.0);

     vec4 diffuse = diffuse_mult_factor*globalcolor*0.4;
     vec4 ambiant = globalcolor*0.25;
     vec4 specular = pow(specular_mult_factor,50)*1.5*globalcolor;

     return ambiant+diffuse+specular;
}

void main(void) {
     vec4 res = vec4(0,0,0,0);
     for(int i=0;i<lightnumber;i++) {
             res+=spotlight(i);
     }
     gl_FragColor = res;
}       