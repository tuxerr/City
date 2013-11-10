#version 410

smooth in vec3 texcoord;

out vec4 pixel_color;

uniform sampler2D tex;
uniform sampler2DArray arraytex;
uniform int choice; //choice == -1 : use tex as texture. else use choice as arraytex layer.

void main() {
    pixel_color = vec4(1,1,0,1);
    if(choice==-1) {
        pixel_color=texture(tex,texcoord.xy);
    } else {
        pixel_color=texture(arraytex,vec3(texcoord.xy,choice));
    }
    pixel_color = vec4(1,0,0,1);
}
