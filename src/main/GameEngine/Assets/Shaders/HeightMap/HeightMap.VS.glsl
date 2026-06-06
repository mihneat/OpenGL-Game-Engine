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

uniform int is_in_game_view;
uniform int is_in_play_mode;

uniform int is_shadow_pass;

// Output
out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;
out vec3 frag_tangent;
out vec3 frag_bitangent;

out mat3 frag_TBN;

out vec3 world_position;
out vec3 world_normal;

out vec3 selection_color;

float get_terrain_height(vec2 tex_coords)
{
    const float water_level = 0.1f;
    
    // Consider texture_2 to be the heightmap (the red channel)
    float height = texture2D(texture_2, tex_scale * tex_coords).r;
    height = max(height, water_level);

    // Raise the terrain based on the height map
    return height * 100.0f;
}

void main()
{
    const float water_level = 0.1f;
    
    // Raise the terrain based on the height map
    vec3 new_local_pos = v_position + vec3(0, 0, get_terrain_height(v_texture_coord));
    
    // Move the coords to world space
    new_local_pos = (Model * vec4(new_local_pos, 1)).xyz;
    
    selection_color = vec3(new_local_pos.x, 0, new_local_pos.z);
    
    // Compute the normal
    const float sample_scale = 2.0f;
    const float frag_count = 1000;
    const float deltaUV = sample_scale * 1.0f / frag_count;
    const float deltaPos = sample_scale * 2.0f / frag_count;
    vec2 up_sample_pos = tex_scale * (v_texture_coord + vec2(0, -deltaUV));
    vec2 down_sample_pos = tex_scale * (v_texture_coord + vec2(0, deltaUV));
    vec2 left_sample_pos = tex_scale * (v_texture_coord + vec2(-deltaUV, 0));
    vec2 right_sample_pos = tex_scale * (v_texture_coord + vec2(deltaUV, 0));

    float up_sample = get_terrain_height(up_sample_pos);
    float down_sample = get_terrain_height(down_sample_pos);
    float left_sample = get_terrain_height(left_sample_pos);
    float right_sample = get_terrain_height(right_sample_pos);

    vec3 up_pos = v_position + vec3(0, deltaPos, 0);
    up_pos = (Model * vec4(up_pos, 1)).xyz;
    vec3 down_pos = v_position + vec3(0, -deltaPos, 0);
    down_pos = (Model * vec4(down_pos, 1)).xyz;
    vec3 left_pos = v_position + vec3(-deltaPos, 0, 0);
    left_pos = (Model * vec4(left_pos, 1)).xyz;
    vec3 right_pos = v_position + vec3(deltaPos, 0, 0);
    right_pos = (Model * vec4(right_pos, 1)).xyz;

    vec3 vertical_vector = normalize(vec3(down_pos.x, down_sample, down_pos.z) - vec3(up_pos.x, up_sample, up_pos.z));
    vec3 horizontal_vector = normalize(vec3(left_pos.x, left_sample, left_pos.z) - vec3(right_pos.x, right_sample, right_pos.z));
    
    vec3 new_normal = normalize(cross(horizontal_vector, vertical_vector));
    
    // Compute the new tangent and bitangent vectors
    vec3 tangent = normalize(vertical_vector);
    vec3 bitangent = normalize(horizontal_vector);
    
    world_position = new_local_pos;
    world_normal = new_normal;

    if (is_in_play_mode == 1) {
        // Curve the terrain
        float dist = distance(helicopter_position, new_local_pos);
        new_local_pos.y -= dist * dist * bend_factor;
    }

    selection_color = vec3(new_local_pos.x, 0, new_local_pos.z);
    
    frag_normal = new_normal;
    frag_color = v_color;
    tex_coord = v_texture_coord;
    frag_tangent = tangent;
    frag_bitangent = bitangent;

    vec3 T = tangent;
    vec3 B = bitangent;
    vec3 N = world_normal;
    frag_TBN = mat3(T, B, N);

    if (is_shadow_pass == 1) {
        gl_Position = Projection * View * vec4(world_position, 1.0);
        return;
    }

    gl_Position = Projection * View * vec4(new_local_pos, 1.0);
}
