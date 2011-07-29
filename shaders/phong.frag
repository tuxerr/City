#version 330

out vec4 pixel_color;

in vec3 color;
smooth in vec4 vert_pos;
smooth in vec4 vert_normal;
smooth in vec2 texcoord;

uniform Light_ {
    int light_type;
    float intensity; 
    vec3 spot_values; //linear_dissipation,illu_angle,max_illu_angle
    vec3 origin; 
    vec3 color; 
    vec3 direction;
    mat4 matrix;
} Light[8];

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    vec3 camera_pos;
} GlobalValues;

uniform int lightnumber;
uniform sampler2DShadow shadowmap;

vec4 spotlight(int lightID) {
     vec4 globalcolor = (vec4(color,1.0)*vec4(Light[lightID].color,1.0));

     vec3 light_ray = vert_pos.xyz-Light[lightID].origin;
     vec3 norm_normal = normalize(vert_normal.xyz);
     vec3 reflected_ray = reflect(normalize(light_ray),norm_normal);
     vec3 eye_ray = GlobalValues.camera_pos-vert_pos.xyz;
     float distance = abs(length(Light[lightID].origin-GlobalValues.camera_pos));

     vec4 ambiant = globalcolor*0.1;
     vec4 diffuse = vec4(0,0,0,0);
     vec4 specular = vec4(0,0,0,0);

     float illu_angle = Light[lightID].spot_values.y;
     float max_illu_angle = Light[lightID].spot_values.z;
     vec3 direction = Light[lightID].direction;
     
     float spot_angle = dot(normalize(light_ray),normalize(direction));
     if(spot_angle>illu_angle) {
         float diffuse_mult_factor = dot(normalize(-light_ray),norm_normal);
         float specular_mult_factor = max(dot(normalize(eye_ray),reflected_ray),0.0);
         diffuse = diffuse_mult_factor*globalcolor*0.4;
         specular = pow(specular_mult_factor,500)*1*globalcolor;
         specular = specular/(distance*Light[lightID].spot_values.x);
         
         if(spot_angle<max_illu_angle) {
             float attenuation = (1-(spot_angle-max_illu_angle)/(illu_angle-max_illu_angle));
             diffuse=diffuse*attenuation;
             specular=specular*attenuation;
         }
     } 
     
//    return ambiant+(diffuse+specular)*Light[lightID].intensity;
    return ambiant+diffuse+specular;
}

vec4 pointlight(int lightID) {
    vec4 globalcolor = (vec4(color,1.0)+vec4(Light[lightID].color,1.0))*0.5;
    
    vec3 light_ray = vert_pos.xyz-Light[lightID].origin;
    vec3 norm_normal = normalize(vert_normal.xyz);
    vec3 reflected_ray = reflect(normalize(light_ray),norm_normal);
    vec3 eye_ray = GlobalValues.camera_pos-vert_pos.xyz;
    float distance = abs(length(Light[lightID].origin-GlobalValues.camera_pos));

    float diffuse_mult_factor = dot(normalize(-light_ray),norm_normal);
    float specular_mult_factor = max(dot(normalize(eye_ray),normalize(reflected_ray)),0.0);

    vec4 ambiant = globalcolor*0.1;
    vec4 diffuse = diffuse_mult_factor*globalcolor*0.4;
    vec4 specular = pow(specular_mult_factor,500)*globalcolor;
    specular = specular/(distance/3*Light[lightID].spot_values.x);
        
//    return ambiant+(diffuse+specular)*Light[lightID].intensity;
    return ambiant+diffuse+specular;
}

vec4 directionallight(int lightID) {
    vec4 globalcolor = (vec4(color,1.0)+vec4(Light[lightID].color,1.0))*0.5;
    
    vec3 light_ray = Light[lightID].direction;
    vec3 norm_normal = normalize(vert_normal.xyz);
    vec3 reflected_ray = reflect(normalize(light_ray),norm_normal);
    vec3 eye_ray = GlobalValues.camera_pos-vert_pos.xyz;

    float diffuse_mult_factor = dot(normalize(-light_ray),norm_normal);
    float specular_mult_factor = max(dot(normalize(eye_ray),normalize(reflected_ray)),0.0);

    vec4 ambiant = globalcolor*0.1;
    vec4 diffuse = diffuse_mult_factor*globalcolor*0.4;
    vec4 specular = pow(specular_mult_factor,500)*globalcolor;
    specular = specular/3*Light[lightID].spot_values.x;
    
//    return ambiant+(diffuse+specular)*Light[lightID].intensity;
    return ambiant+diffuse+specular;
}

void main(void) {
     vec4 res = vec4(0,0,0,0);
     for(int i=0;i<lightnumber;i++) {
         if(Light[i].light_type==1) {
             res+=pointlight(i);
         } else if(Light[i].light_type==2) {
             res+=spotlight(i);
         } else if(Light[i].light_type==3) {
             vec4 light_point = Light[i].matrix*vert_pos;
             light_point.x = (light_point.x/2)+0.5;
             light_point.y = (light_point.y/2)+0.5;
             light_point.z = (light_point.z/2)+0.5;
             float lightval = texture(shadowmap,light_point.xyz);
             if(light_point.z<=lightval) {
                 res+=directionallight(i);
             }
         }
     }
     pixel_color = res;
}       
