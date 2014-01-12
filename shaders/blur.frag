#version 410

smooth in vec2 tex_position;
smooth in vec2 tex_positions[14];

out vec4 pixel_color;

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
uniform sampler2D deferred_colormap;

void main() {
    pixel_color = vec4(0.0);
    pixel_color += texture(deferred_colormap, tex_positions[0])*0.0044299121055113265;
    pixel_color += texture(deferred_colormap, tex_positions[1])*0.00895781211794;
    pixel_color += texture(deferred_colormap, tex_positions[2])*0.0215963866053;
    pixel_color += texture(deferred_colormap, tex_positions[3])*0.0443683338718;
    pixel_color += texture(deferred_colormap, tex_positions[4])*0.0776744219933;
    pixel_color += texture(deferred_colormap, tex_positions[5])*0.115876621105;
    pixel_color += texture(deferred_colormap, tex_positions[6])*0.147308056121;
    pixel_color += texture(deferred_colormap, tex_position    )*0.159576912161;
    pixel_color += texture(deferred_colormap, tex_positions[7])*0.147308056121;
    pixel_color += texture(deferred_colormap, tex_positions[8])*0.115876621105;
    pixel_color += texture(deferred_colormap, tex_positions[9])*0.0776744219933;
    pixel_color += texture(deferred_colormap, tex_positions[10])*0.0443683338718;
    pixel_color += texture(deferred_colormap, tex_positions[11])*0.0215963866053;
    pixel_color += texture(deferred_colormap, tex_positions[12])*0.00895781211794;
    pixel_color += texture(deferred_colormap, tex_positions[13])*0.0044299121055113265;
}
