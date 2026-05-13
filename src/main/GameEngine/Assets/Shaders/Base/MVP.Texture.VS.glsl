#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;
layout(location = 4) in vec3 v_tangent;
layout(location = 5) in vec3 v_bitangent;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec2 tex_scale;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform sampler2D texture_4;

uniform int is_in_game_view;
uniform int is_in_play_mode;

uniform int is_shadow_pass;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;
out vec3 frag_tangent;
out vec3 frag_bitangent;

out mat3 frag_TBN;

out vec3 world_position;
out vec3 world_normal;

void main()
{
    world_position = (Model * vec4(v_position, 1)).xyz;
    world_normal = normalize(mat3(Model) * v_normal);

    frag_position = v_position;
    frag_normal = v_normal;
    frag_color = v_color;
    tex_coord = v_texture_coord;
    frag_tangent = v_tangent;
    frag_bitangent = v_bitangent;

    vec3 T = normalize(vec3(Model * vec4(v_tangent,   0.0)));
    vec3 B = normalize(vec3(Model * vec4(v_bitangent, 0.0)));
    vec3 N = world_normal;
    frag_TBN = mat3(T, B, N);

    gl_Position = Projection * View * vec4(world_position, 1.0);
}
