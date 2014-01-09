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

uniform sampler2D normalmap;
uniform sampler2D depthmap;
uniform sampler2D colormap;

vec3 color;
vec4 vert_pos;
vec4 vert_normal;
vec3 texcoord;
vec2 deferred_texcoord;

vec4 spotlight() {
    vec4 globalcolor = (vec4(color,1.0)*vec4(Light.color,1.0));

    vec3 light_ray = vert_pos.xyz-Light.origin;
    vec3 norm_normal = normalize(vert_normal.xyz);
    vec3 reflected_ray = reflect(normalize(light_ray),norm_normal);
    vec3 eye_ray = GlobalValues.camera_pos-vert_pos.xyz;
    float distance = abs(length(Light.origin-GlobalValues.camera_pos));

    vec4 ambiant = globalcolor*0.1;
    vec4 diffuse = vec4(0,0,0,0);
    vec4 specular = vec4(0,0,0,0);

    float illu_angle = Light.spot_values.y;
    float max_illu_angle = Light.spot_values.z;
    vec3 direction = Light.direction;
     
    float spot_angle = dot(normalize(light_ray),normalize(direction));
    if(spot_angle>illu_angle) {
        float diffuse_mult_factor = dot(normalize(-light_ray),norm_normal);
        float specular_mult_factor = max(dot(normalize(eye_ray),reflected_ray),0.0);
        diffuse = diffuse_mult_factor*globalcolor*0.4;
        specular = pow(specular_mult_factor,500.0)*1*globalcolor;
        specular = specular/(distance*Light.spot_values.x);
         
        if(spot_angle<max_illu_angle) {
            float attenuation = (1-(spot_angle-max_illu_angle)/(illu_angle-max_illu_angle));
            diffuse=diffuse*attenuation;
            specular=specular*attenuation;
        }
    } 
     
//    return ambiant+(diffuse+specular)*Light.intensity;
    return ambiant+diffuse+specular;
}

vec4 pointlight() {
    vec4 globalcolor = (vec4(color,1.0)+vec4(Light.color,1.0))*0.5;
    
    vec3 light_ray = vert_pos.xyz-Light.origin;
    vec3 norm_normal = normalize(vert_normal.xyz);
    vec3 reflected_ray = reflect(normalize(light_ray),norm_normal);
    vec3 eye_ray = GlobalValues.camera_pos-vert_pos.xyz;
    float eye_distance = distance(Light.origin,GlobalValues.camera_pos);
    float light_distance = length(light_ray);
    
    float diffuse_mult_factor=1,specular_mult_factor=0;
    if(length(vert_normal)!=0) { //if not in generated lines
        diffuse_mult_factor = max(dot(normalize(-light_ray),norm_normal),0.0);
        specular_mult_factor = max(dot(normalize(eye_ray),normalize(reflected_ray)),0.0);
    }
    
    vec4 ambiant = globalcolor*0.1;
    vec4 diffuse = diffuse_mult_factor*globalcolor*0.4;
    vec4 specular = pow(specular_mult_factor,500)*globalcolor;
    specular = specular/(eye_distance/3*Light.spot_values.x);
    
    
    float light_distance_pow2 = light_distance*light_distance;
    
    vec4 result = (diffuse+ambiant+specular)*Light.intensity*(1/(1+light_distance+light_distance_pow2+pow(light_distance_pow2,5)/30))*LINEAR_LIGHT_ATTENUATION;
    return result;
}

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
    
    if(Light.light_type==1) {
            pixel_color = pointlight();
    } else if(Light.light_type==2) {
            pixel_color = spotlight();
    }
    //pixel_color=vec4(deferred_texcoord.x,deferred_texcoord.y,0,1);
}
