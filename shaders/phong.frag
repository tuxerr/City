#version 330

layout(std140,row_major) uniform;

in vec3 color;
smooth in vec4 vert_pos;
smooth in vec4 vert_normal;

uniform Light {
   vec3 origin;
   vec3 color;
   vec3 direction;
   float intensity;
   float linear_dissipation;
   float max_illu_angle;
   float illu_angle;
} Lights[8];

uniform GlobalValues {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    vec3 camera_pos;
};

uniform int lightnumber;

vec4 spotlight(int lightID) {
     vec4 globalcolor = (vec4(color,1.0)+vec4(Lights[lightID].color,1.0))*0.5;

     vec3 light_ray = Lights[lightID].origin-vert_pos.xyz;
     vec3 norm_normal = normalize(vert_normal.xyz);
     vec3 reflected_ray = reflect(-normalize(light_ray),norm_normal);
     vec3 eye_ray = camera_pos-vert_pos.xyz;

     vec4 ambiant = globalcolor*0.25;
     vec4 diffuse = vec4(0,0,0,0);
     vec4 specular = vec4(0,0,0,0);


/*     if(Lights[lightID].illu_angle!=-1) {
         return vec4(1,1,1,1);
          float illu_angle = Lights[lightID].illu_angle;
          float max_illu_angle = Lights[lightID].max_illu_angle;
  
          vec4 direction = normal_matrix*vec4(Lights[lightID].direction,1.0);
             
          float spot_angle = acos(dot(normalize(light_ray),normalize(direction.xyz)));
          spot_angle=4;
          if(spot_angle<illu_angle) {
              float diffuse_mult_factor = dot(normalize(light_ray),norm_normal);
              float specular_mult_factor = max(dot(normalize(eye_ray),reflected_ray),0.0);
              diffuse = diffuse_mult_factor*globalcolor*0.4;
              specular = pow(specular_mult_factor,50)*1.5*globalcolor;

         //     if(spot_angle>max_illu_angle) {
//                  float attenuation = (spot_angle-max_illu_angle)/(illu_angle-max_illu_angle);
//                  attenuation=pow(attenuation,2);
//                  diffuse=diffuse*(1-attenuation);
//                  specular=specular*(1-attenuation);
//              }
          }
     
     }  else { //light is a pointlight */
         float diffuse_mult_factor = dot(normalize(light_ray),norm_normal);
         float specular_mult_factor = max(dot(normalize(eye_ray),normalize(reflected_ray)),0.0);
         diffuse = diffuse_mult_factor*globalcolor*0.4;
         specular = pow(specular_mult_factor,500)*1*globalcolor;
         specular = specular/max(length(abs(Lights[lightID].origin-camera_pos)/10),1.0);
         //   }
     return ambiant+diffuse+specular;
}

void main(void) {
     vec4 res = vec4(0,0,0,0);
     for(int i=0;i<lightnumber;i++) {
             res+=spotlight(i);
     }
     gl_FragColor = res;
}       
