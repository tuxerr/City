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
    mat4 matrix1;
    mat4 matrix2;
    mat4 matrix3;
    mat4 matrix4;
    bool render_shadows;
} Light[8];

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    vec3 camera_pos;
    vec3 eye_vector;
} GlobalValues;

uniform int lightnumber;

uniform sampler2DArrayShadow shadowmap[8];
uniform samplerCubeShadow shadowcubemap[8];

uniform float cascaded_shading_zdelta;

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

float directional_shadowing(int lightID) {
    float dotprod = dot(normalize(vert_pos.xyz-GlobalValues.camera_pos),normalize(GlobalValues.eye_vector));

    float zdelta = (distance(vert_pos.xyz,GlobalValues.camera_pos)*dotprod)-1;
    float cascaded_layer=5;
    for(int i=0;i<4;i++) {
        if(zdelta<=cascaded_shading_zdelta*(pow(2,i+1)-1)) {
            cascaded_layer=i;
            break;
        }        
    }
    
    vec4 light_point;
    if(cascaded_layer==0) {
        light_point = Light[lightID].matrix1*vert_pos;    
    } else if(cascaded_layer==1) {
        light_point = Light[lightID].matrix2*vert_pos;    
    } else if(cascaded_layer==2) {
        light_point = Light[lightID].matrix3*vert_pos;    
    } else if(cascaded_layer==3) {
        light_point = Light[lightID].matrix4*vert_pos;    
    }

    light_point.x = (light_point.x/2)+0.5;
    light_point.y = (light_point.y/2)+0.5;
    light_point.z = (light_point.z/2)+0.5;
//    float lightval = texture(shadowmap[lightID],vec4(light_point.xy,cascaded_layer,cascaded_layer))+0.0025;
    float lightval = texture(shadowmap[lightID],vec4(light_point.xy,cascaded_layer,cascaded_layer))+0.0025;
/*  float lightvals1 = textureOffset(shadowmap[lightID],vec4(light_point.xyz,cascaded_layer),ivec2(1,0))+0.0025;
    float lightvals2 = textureOffset(shadowmap[lightID],vec4(light_point.xyz,cascaded_layer),ivec2(-1,0))+0.0025;
    float lightvals3 = textureOffset(shadowmap[lightID],vec4(light_point.xyz,cascaded_layer),ivec2(0,1)+0.0025;
    float lightvals4 = textureOffset(shadowmap[lightID],vec4(light_point.xyz,cascaded_layer),ivec2(0,-1)+0.0025; */

    if(light_point.z <= lightval) {
        return 1.0;
    } else {
        float endres = 0;
/*        if(light_point.z <= lightvals1) {
            endres+=0.25;
        }
        if(light_point.z <= lightvals2) {
            endres+=0.25;
        }
        if(light_point.z <= lightvals3) {
            endres+=0.25;
        }
        if(light_point.z <= lightvals4) {
            endres+=0.25;
            } */
        return endres;
    }

    return 0.0;
}

void main(void) {
     vec4 res = vec4(0,0,0,0);
     for(int i=0;i<lightnumber;i++) {

         if(Light[i].light_type==1) {
             if(Light[i].render_shadows) {
                 vec3 light_ray = vert_pos.xyz-Light[i].origin;
                 float lightval = texture(shadowcubemap[i],vec4(light_ray,1.0))+0.0025;
                 if(abs(light_ray.z) <= lightval) {
                     res+=pointlight(i);
                 }   
             } else {
                 res+=pointlight(i);
             }

         } else if(Light[i].light_type==2) {
             res+=spotlight(i);
         } else if(Light[i].light_type==3) {
             if(Light[i].render_shadows) {
                 res += directionallight(i)*directional_shadowing(i);
             } else {
                 res += directionallight(i);
             }

         }
     }
     pixel_color = res;
}       
