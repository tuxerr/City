#version 410

in vec3 in_Vertex;

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

smooth out vec2 tex_position;
smooth out vec2 tex_positions[14];

void main() {
    gl_Position = vec4(in_Vertex.xy,0.,1.0);
    tex_position = gl_Position.xy/2+0.5;
    
    float vert_displacement = 1./GlobalValues.screen_size.y;
    
    tex_positions[0] = tex_position + vec2(0,vert_displacement*7);
    tex_positions[1] = tex_position + vec2(0,vert_displacement*6);
    tex_positions[2] = tex_position + vec2(0,vert_displacement*5);
    tex_positions[3] = tex_position + vec2(0,vert_displacement*4);
    tex_positions[4] = tex_position + vec2(0,vert_displacement*3);
    tex_positions[5] = tex_position + vec2(0,vert_displacement*2);
    tex_positions[6] = tex_position + vec2(0,vert_displacement*1);
    tex_positions[7] = tex_position + vec2(0,-vert_displacement);
    tex_positions[8] = tex_position + vec2(0,-vert_displacement*2);
    tex_positions[9] = tex_position + vec2(0,-vert_displacement*3);
    tex_positions[10] = tex_position + vec2(0,-vert_displacement*4);
    tex_positions[11] = tex_position + vec2(0,-vert_displacement*5);
    tex_positions[12] = tex_position + vec2(0,-vert_displacement*6);
    tex_positions[13] = tex_position + vec2(0,-vert_displacement*7);
}
