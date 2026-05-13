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

uniform vec3 helicopter_position;
uniform float bend_factor;
uniform int is_helicopter;

uniform int is_in_game_view;
uniform int is_in_play_mode;

uniform int ignore_water;

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

vec2 compute_sample_point(vec3 world_pos)
{
    // Formula: -1 * sample_position / (500 * 2) + 0.5, where
    // 500 = the scale of the ground
    // -1 due of the rotation of the ground plane
    // We're trying to brint the interval [-500, 500] to [0, 1], the UV values of the ground plane
    return vec2(-world_pos.x, world_pos.z) / 1000.0f + 0.5f;
}

void main()
{
    const float water_level = 0.1f;
    
    // Sample a point on the heightmap
    vec2 heightmap_sample_point = is_helicopter == 1 ? 
        compute_sample_point(helicopter_position) :
        compute_sample_point((Model * vec4(v_position, 1)).xyz);

    // Consider texture_2 to be the heightmap (the red channel)
    float height = texture2D(texture_2, heightmap_sample_point).r;
    
    if (ignore_water == 0)
        height = max(height, water_level);
    
    // Move the coords to world space
    vec3 new_local_pos = (Model * vec4(v_position, 1)).xyz;

    // Raise the vertices based on the height map
    new_local_pos = new_local_pos + vec3(0, height * 100.0f, 0);

    world_position = new_local_pos;
    world_normal = normalize(mat3(Model) * v_normal);

    if (is_in_play_mode == 1) {
        // Curve the mesh
        float dist = distance(helicopter_position, new_local_pos);
        new_local_pos.y -= dist * dist * bend_factor;
    }

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

    if (is_shadow_pass == 1) {
        gl_Position = Projection * View * vec4(world_position, 1.0);
        return;
    }

    gl_Position = Projection * View * vec4(new_local_pos, 1.0);
}
