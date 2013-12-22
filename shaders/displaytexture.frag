#version 410

out vec4 pixel_color;

uniform sampler2D tex;
uniform sampler2DArray arraytex;
uniform int choice; //choice == -1 : use tex as texture. else use choice as arraytex layer. -2 : depth texture (replicate)

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

void main() {
    vec2 texcoord = gl_FragCoord.xy/GlobalValues.screen_size;

    if(choice==-1) {
        pixel_color=texture(tex,texcoord);
    } else if(choice==-2) {
        vec4 depth_val=texture(tex,texcoord);
        if(depth_val.x!=1) {
            depth_val=vec4(0,0,0,0);
        }
        pixel_color=vec4(depth_val.x,depth_val.x,depth_val.x,1);
    } else {
        pixel_color=texture(arraytex,vec3(texcoord,choice));
    }
}
