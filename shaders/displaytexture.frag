#version 330

smooth in texcoord;

out vec4 pixel_color;
uniform sampler2D tex;
uniform sampler2DArray arraytex;
uniform int choice; //choice == -1 : use tex as texture. else use choice as arraytex layer.

void main() {
    if(choice==-1) {
        pixel_color=texture(tex,texcoord);
    } else {
        pixel_color=texture(arraytex,vec3(texcoord,choice));
    }
}
