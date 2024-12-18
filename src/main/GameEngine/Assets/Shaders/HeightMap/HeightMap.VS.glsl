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

uniform int is_in_game_view;
uniform int is_in_play_mode;

uniform int is_shadow_pass;

// Output
out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;

out vec3 world_position;
out vec3 world_normal;

out vec3 selection_color;

void main()
{
    const float water_level = 0.1f;
    
    // Consider texture_2 to be the heightmap (the red channel)
    float height = texture2D(texture_2, tex_scale * v_texture_coord).r;
    height = max(height, water_level);
    
    // Raise the terrain based on the height map
    vec3 new_local_pos = v_position + vec3(0, 0, height * 100.0f);
    
    // Move the coords to world space
    new_local_pos = (Model * vec4(new_local_pos, 1)).xyz;
    
    selection_color = vec3(new_local_pos.x, 0, new_local_pos.z);
    
    // Compute the normal
    vec2 up_sample_pos = tex_scale * (v_texture_coord + vec2(0, 0.01f));
    vec2 down_sample_pos = tex_scale * (v_texture_coord + vec2(0, -0.01f));
    vec2 left_sample_pos = tex_scale * (v_texture_coord + vec2(-0.01f, 0));
    vec2 right_sample_pos = tex_scale * (v_texture_coord + vec2(0.01f, 0));
    
    float up_sample = max(texture2D(texture_2, up_sample_pos).r, water_level);
    float down_sample = max(texture2D(texture_2, down_sample_pos).r, water_level);
    float left_sample = max(texture2D(texture_2, left_sample_pos).r, water_level);
    float right_sample = max(texture2D(texture_2, right_sample_pos).r, water_level);

    vec3 vertical_vector = normalize(vec3(down_sample_pos.x, down_sample, down_sample_pos.y) - vec3(up_sample_pos.x, up_sample, up_sample_pos.y));
    vec3 horizontal_vector = normalize(vec3(left_sample_pos.x, left_sample, left_sample_pos.y) - vec3(right_sample_pos.x, right_sample, right_sample_pos.y));
    
    vec3 new_normal = cross(vertical_vector, horizontal_vector);
    
    world_position = new_local_pos;
    world_normal = normalize(mat3(Model) * new_normal);

    if (is_in_play_mode == 1) {
        // Curve the terrain
        float dist = distance(helicopter_position, new_local_pos);
        new_local_pos.y -= dist * dist * bend_factor;
    }

    selection_color = vec3(new_local_pos.x, 0, new_local_pos.z);
    
    frag_normal = new_normal;
    frag_color = v_color;
    tex_coord = v_texture_coord;

    if (is_shadow_pass == 1) {
        gl_Position = Projection * View * vec4(world_position, 1.0);
        return;
    }

    gl_Position = Projection * View * vec4(new_local_pos, 1.0);
}
