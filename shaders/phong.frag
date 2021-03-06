#version 410

#define LINEAR_LIGHT_ATTENUATION 1

out vec4 pixel_color;

uniform Light {
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
    float shadow_min_range;
    float shadow_max_range;
} Light;

uniform GlobalValues {
    mat4 modelview; // camera*modelview
    mat4 projection_modelview; //perspective*camera*modelview
    mat4 normal_matrix; // transpose(inverse(modelview))
    mat4 screen_to_world; //inverse(perspective*camera)
    float near;
    vec3 camera_pos;
    vec3 eye_vector;
    float far;
    vec2 screen_size;
} GlobalValues;

uniform sampler2DArrayShadow shadowmap;
//uniform samplerCubeShadow shadowcubemap[8];
uniform sampler2D normalmap;
uniform sampler2D depthmap;
uniform sampler2D colormap;
uniform sampler2D texcoordmap;

uniform float cascaded_shading_zdelta;

vec3 color;
vec4 vert_pos;
vec4 vert_normal;
vec3 texcoord;
vec2 deferred_texcoord;

float directional_shadowing() {
    float shadow_min_range = Light.shadow_min_range;
    if(shadow_min_range==-1) {
        shadow_min_range=GlobalValues.near;
    }

    float distance_from_cam = length(vert_pos.xyz-GlobalValues.camera_pos);
    float zdelta = dot(vert_pos.xyz-GlobalValues.camera_pos,normalize(GlobalValues.eye_vector))-shadow_min_range;

    if(Light.shadow_max_range != -1 && zdelta>=Light.shadow_max_range) {
        return 1.0;
    }
    if(zdelta<=Light.shadow_min_range) {
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
        light_point = Light.matrix1*vert_pos;    
    } else if(cascaded_layer==1) {
        light_point = Light.matrix2*vert_pos;    
    } else if(cascaded_layer==2) {
        light_point = Light.matrix3*vert_pos;    
    } else if(cascaded_layer==3) {
        light_point = Light.matrix4*vert_pos;    
    } else if(cascaded_layer==4) {
        light_point = Light.matrix5*vert_pos;
    } else if(cascaded_layer==5) {
        light_point = Light.matrix6*vert_pos;    
    } else if(cascaded_layer==6) {
        light_point = Light.matrix7*vert_pos;    
    } else if(cascaded_layer==7) {
        light_point = Light.matrix8*vert_pos;    
    }
    
    // clamping from [1,-1] to [1,0]
    light_point.x = (light_point.x/2)+0.5;
    light_point.y = (light_point.y/2)+0.5;
    light_point.z = (light_point.z/2)+0.5;
    float lightval;
    float depth_offset_u20=0.0003;
    float depth_offset_o20=0.001;
    float depth_offset_final = (distance_from_cam<10? depth_offset_u20 : depth_offset_o20);
    lightval = texture(shadowmap,vec4(light_point.xy,cascaded_layer,cascaded_layer))+depth_offset_final;

    float soft_offset = 0.0009765625;
    vec4 v4off1 = vec4(light_point.xy+vec2(soft_offset,0),cascaded_layer,cascaded_layer);
    vec4 v4off2 = vec4(light_point.xy+vec2(-soft_offset,0),cascaded_layer,cascaded_layer);
    vec4 v4off3 = vec4(light_point.xy+vec2(0,soft_offset),cascaded_layer,cascaded_layer);
    vec4 v4off4 = vec4(light_point.xy+vec2(0,-soft_offset),cascaded_layer,cascaded_layer);
    float lightval1,lightval2,lightval3,lightval4;

    lightval1 = texture(shadowmap,v4off1)+depth_offset_final;
    lightval2 = texture(shadowmap,v4off2)+depth_offset_final;
    lightval3 = texture(shadowmap,v4off3)+depth_offset_final;
    lightval4 = texture(shadowmap,v4off4)+depth_offset_final;
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

vec4 directionallight() {
    vec4 globalcolor = (vec4(color,1.0)*vec4(Light.color,1.0));
    if(globalcolor.rgb!=vec3(0,0,0)) {
    
        vec3 light_ray = Light.direction;
        vec3 norm_normal = normalize(vert_normal.xyz);
        vec3 reflected_ray = reflect(normalize(light_ray),norm_normal);
        vec3 eye_ray = GlobalValues.camera_pos-vert_pos.xyz;

        float diffuse_mult_factor=1,specular_mult_factor=0;
        if(length(vert_normal)!=0) { //if not in generated lines
            float diffuse_mult_factor = max(dot(normalize(-light_ray),norm_normal),0.0);
            float specular_mult_factor = max(dot(normalize(eye_ray),normalize(reflected_ray)),0.0);
        }

        vec4 ambiant = globalcolor*0.3;
        vec4 diffuse = diffuse_mult_factor*globalcolor*0.7;
        vec4 specular = pow(specular_mult_factor,500)*vec4(Light.color,1.0);
        specular = specular/3*Light.spot_values.x;
    
        if(Light.render_shadows) {
            float res = directional_shadowing();
            vec4 fullillu = ambiant+diffuse+specular;
            vec4 shadowed = ambiant;
            return mix(shadowed,fullillu,res);
        } else {
            return ambiant+diffuse+specular;
        }
        
    } else {
        return globalcolor;
    }

}

/*vec4 apply_fog(vec4 color,float distance) {

    const float LOG2 = 1.442695;
    float fogFactor = exp2( -FOG_DENSITY * 
                            FOG_DENSITY * 
                            distance * 
                            distance * 
                            LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    return mix(vec4(1,0.9,0.7,1), color, fogFactor);
}*/

void main(void) {
    // getting information from deferred textures
    deferred_texcoord = gl_FragCoord.xy/GlobalValues.screen_size;

    color = texture(colormap,deferred_texcoord).rgb;
    
    float depth=texture(depthmap,deferred_texcoord).r;
    vec4 screen_coordinates = vec4(deferred_texcoord.xy*2-1,(depth-0.5)*2,1.);
    vert_pos = GlobalValues.screen_to_world*screen_coordinates;
    vert_pos.xyz=vert_pos.xyz/vert_pos.w;
    vert_pos.w=1;

    vert_normal = texture(normalmap,deferred_texcoord);
    
    texcoord = texture(texcoordmap,deferred_texcoord).xyz;

    pixel_color = directionallight();

    //pixel_color=vec4(deferred_texcoord.x,deferred_texcoord.y,0,1);
//    pixel_color = apply_fog(res,distance(GlobalValues.camera_pos,vert_pos.xyz));

}       
