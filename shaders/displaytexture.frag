#version 410

smooth in vec3 texcoord;

out vec4 pixel_color;

uniform sampler2D tex;
uniform sampler2DArray arraytex;
uniform int choice; //choice == -1 : use tex as texture. else use choice as arraytex layer. -2 : depth texture (replicate)

void main() {
    if(choice==-1) {
        pixel_color=texture(tex,texcoord.xy);
    } else if(choice==-2) {
        vec4 depth_val=texture(tex,texcoord.xy);
        pixel_color=vec4(depth_val.x,depth_val.x,depth_val.x,0.5);
    }else {
        pixel_color=texture(arraytex,vec3(texcoord.xy,choice));
    }
}
