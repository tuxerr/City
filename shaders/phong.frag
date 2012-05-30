#version 330

#define FOG_DENSITY 0.001

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
    mat4 matrix5;
    mat4 matrix6;
    mat4 matrix7;
    mat4 matrix8;
    bool render_shadows;
    vec4 offset2;
    float shadow_min_range;
    vec4 offset;
    float shadow_max_range;
} Light[8];

uniform GlobalValues_ {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    float near;
    vec3 camera_pos;
    vec3 eye_vector;
    float far;
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
        specular = pow(specular_mult_factor,500.0)*1*globalcolor;
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



float directional_shadowing(int lightID) {
    float shadow_min_range = Light[lightID].shadow_min_range;
    if(shadow_min_range==-1) {
        shadow_min_range=GlobalValues.near;
    }

    float distance_from_cam = length(vert_pos.xyz-GlobalValues.camera_pos);
    float zdelta = dot(vert_pos.xyz-GlobalValues.camera_pos,normalize(GlobalValues.eye_vector))-shadow_min_range;

    if(Light[lightID].shadow_max_range != -1 && zdelta>=Light[lightID].shadow_max_range) {
        return 1.0;
    }
    if(zdelta<=Light[lightID].shadow_min_range) {
        return 1.0;
    }

    int cascaded_layer=9;
    for(int i=0;i<8;i++) {
        if(zdelta<=cascaded_shading_zdelta*(pow(2.0,i+1.0)-shadow_min_range)) {
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
    } else if(cascaded_layer==4) {
        light_point = Light[lightID].matrix5*vert_pos;    
    } else if(cascaded_layer==5) {
        light_point = Light[lightID].matrix6*vert_pos;    
    } else if(cascaded_layer==6) {
        light_point = Light[lightID].matrix7*vert_pos;    
    } else if(cascaded_layer==7) {
        light_point = Light[lightID].matrix8*vert_pos;    
    }
    
    // clamping from [1,-1] to [1,0]
    light_point.x = (light_point.x/2)+0.5;
    light_point.y = (light_point.y/2)+0.5;
    light_point.z = (light_point.z/2)+0.5;
    float lightval;
    float depth_offset_u20=0.0003;
    float depth_offset_o20=0.001;
    if(distance_from_cam<10) {
        lightval = texture(shadowmap[lightID],vec4(light_point.xy,cascaded_layer,cascaded_layer))+depth_offset_u20;
    } else {
        lightval = texture(shadowmap[lightID],vec4(light_point.xy,cascaded_layer,cascaded_layer))+depth_offset_o20;
    }

    float soft_offset = 0.0009765625;
    vec4 v4off1 = vec4(light_point.xy+vec2(soft_offset,0),cascaded_layer,cascaded_layer);
    vec4 v4off2 = vec4(light_point.xy+vec2(-soft_offset,0),cascaded_layer,cascaded_layer);
    vec4 v4off3 = vec4(light_point.xy+vec2(0,soft_offset),cascaded_layer,cascaded_layer);
    vec4 v4off4 = vec4(light_point.xy+vec2(0,-soft_offset),cascaded_layer,cascaded_layer);
    float lightval1,lightval2,lightval3,lightval4;

    if(distance_from_cam<10) {
        lightval1 = texture(shadowmap[lightID],v4off1)+depth_offset_u20;
        lightval2 = texture(shadowmap[lightID],v4off2)+depth_offset_u20;
        lightval3 = texture(shadowmap[lightID],v4off3)+depth_offset_u20;
        lightval4 = texture(shadowmap[lightID],v4off4)+depth_offset_u20;
    } else {
        lightval1 = texture(shadowmap[lightID],v4off1)+depth_offset_o20;
        lightval2 = texture(shadowmap[lightID],v4off2)+depth_offset_o20;
        lightval3 = texture(shadowmap[lightID],v4off3)+depth_offset_o20;
        lightval4 = texture(shadowmap[lightID],v4off4)+depth_offset_o20;
    }

    if(light_point.z <= lightval) {
        return 1.0;
    } else {
        float endres = 0;
        if(light_point.z <= lightval1) {
            endres+=0.25;
        }
        if(light_point.z <= lightval2) {
            endres+=0.25;
        }
        if(light_point.z <= lightval3) {
            endres+=0.25;
        }
        if(light_point.z <= lightval4) {
            endres+=0.25;
        } 
        return endres;
    }

    return 0.0;
}

vec4 directionallight(int lightID) {
    vec4 globalcolor = (vec4(color,1.0)*vec4(Light[lightID].color,1.0));
    
    vec3 light_ray = Light[lightID].direction;
    vec3 norm_normal = normalize(vert_normal.xyz);
    vec3 reflected_ray = reflect(normalize(light_ray),norm_normal);
    vec3 eye_ray = GlobalValues.camera_pos-vert_pos.xyz;

    float diffuse_mult_factor = max(dot(normalize(-light_ray),norm_normal),0.0);
    float specular_mult_factor = max(dot(normalize(eye_ray),normalize(reflected_ray)),0.0);

    vec4 ambiant = globalcolor*0.3;
    vec4 diffuse = diffuse_mult_factor*globalcolor*0.7;
    vec4 specular = pow(specular_mult_factor,500)*vec4(Light[lightID].color,1.0);
    specular = specular/3*Light[lightID].spot_values.x;
    
//    return ambiant+(diffuse+specular)*Light[lightID].intensity;
    if(Light[lightID].render_shadows) {
        float res = directional_shadowing(lightID);
        vec4 fullillu = ambiant+diffuse+specular;
        vec4 shadowed = ambiant;
        return mix(shadowed,fullillu,res);
    } else {
        return ambiant+diffuse+specular;
    }

}

vec4 apply_fog(vec4 color,float distance) {

    const float LOG2 = 1.442695;
    float fogFactor = exp2( -FOG_DENSITY * 
                            FOG_DENSITY * 
                            distance * 
                            distance * 
                            LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    return mix(vec4(1,0.9,0.7,1), color, fogFactor);
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
            res += directionallight(i);
        }
    }

//    pixel_color=res;
    pixel_color = apply_fog(res,distance(GlobalValues.camera_pos,vert_pos.xyz));
}       
