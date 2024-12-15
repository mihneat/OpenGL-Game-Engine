#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec2 tex_scale;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform sampler2D texture_4;

uniform vec3 helicopter_position;
uniform float bend_factor;
uniform int is_helicopter;

uniform int is_in_game_view;
uniform int is_in_play_mode;

// Output
out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;

out vec3 world_position;
out vec3 world_normal;

void main()
{
    // Move the coords to world space
    vec3 new_local_pos = (Model * vec4(v_position, 1)).xyz;
    
    // Lock the skybox to the helicopter
    new_local_pos += helicopter_position;

    world_position = new_local_pos;
    world_normal = normalize(mat3(Model) * v_normal);

    frag_normal = v_normal;
    frag_color = v_color;
    tex_coord = v_texture_coord + vec2(-helicopter_position.z, helicopter_position.x) * 0.0005f;

    gl_Position = Projection * View * vec4(new_local_pos, 1.0);
}
