#version 330

// Input
in vec3 frag_position;
in vec3 frag_color;

in vec3 world_position;
in vec3 world_normal;

in vec2 tex_coord;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_selection;

// Uniform 
uniform vec3 eye_position;

uniform vec4  fog_color;
uniform bool  render_fog;

uniform int is_scrolling;
uniform vec2 scroll_amount;

uniform int use_texture;
uniform vec2 tex_scale;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform sampler2D texture_4;
//uniform sampler2D texture_normal;

uniform vec4 mesh_color;

uniform float selection_value;

uniform int distance_from_leaf;

uniform vec3 helicopter_position;

vec3 get_light_contribution();
float get_fog_factor(float dist);

vec4 compute_texture()
{    
    // Compute UV value
    vec2 uv = vec2(
        (1.0f / (2.0f * 3.14159f)) * atan(frag_position.z, frag_position.x),
        frag_position.y / 2.0f + 0.5f
    );
    
    vec4 bark_tex = texture2D(texture_1, tex_scale * uv);
    
    // Only the bark
    if (distance_from_leaf > 1)
        return bark_tex;

    vec4 corona_tex = texture2D(texture_3, tex_scale * uv);
    
    // Mix with corona
    if (distance_from_leaf == 1)
        return mix(bark_tex, corona_tex, uv.y);
    
    // Only the corona
    return corona_tex;
}

void main()
{
    vec3 light = get_light_contribution();
    
    // Apply light to color
    vec4 tex = compute_texture();
    if (tex.a * mesh_color.a < 0.5)
        discard;

    // Light * texture * color
    vec4 lit_vertex = vec4(light * tex.xyz * mesh_color.xyz, 1);

    // Apply fog calculations and output them
    // float alpha = (render_fog == true) ? get_fog_factor(distance(eye_position, world_position)) : 0.0;
    float alpha = get_fog_factor(distance(helicopter_position, world_position));
    out_color = mix(lit_vertex, fog_color, alpha);

    // out_color = vec4(selection_value - 1.0f);
    out_selection = vec4(0, 0, 0, selection_value);
}
