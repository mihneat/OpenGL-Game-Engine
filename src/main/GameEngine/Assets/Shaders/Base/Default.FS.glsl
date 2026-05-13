#version 330

// Input
in vec3 frag_position;
in vec3 frag_color;

in vec3 world_position;
in vec3 world_normal;

in vec2 tex_coord;

in vec3 frag_tangent;
in vec3 frag_bitangent;

in mat3 frag_TBN;

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

uniform int use_normal_maps;
uniform sampler2D normal_1;
uniform sampler2D normal_2;
uniform sampler2D normal_3;
uniform sampler2D normal_4;

uniform vec4 mesh_color;

uniform float selection_value;
        
uniform vec3 helicopter_position;

vec3 get_light_contribution(vec3 normal);
float get_fog_factor(float dist);

void main()
{
    vec3 light = get_light_contribution(world_normal);

    // Apply light to color
    vec4 tex = texture2D(texture_1, tex_scale * tex_coord);
    if (tex.a * mesh_color.a < 0.5)
        discard;

    // Light * texture * color
    vec4 lit_vertex = vec4(light * tex.xyz * mesh_color.xyz, 1);

    // Apply fog calculations and output them
    // float alpha = (render_fog == true) ? get_fog_factor(distance(eye_position, world_position)) : 0.0;
    // float alpha = get_fog_factor(distance(helicopter_position, world_position));
    out_color = lit_vertex; // mix(lit_vertex, fog_color, alpha);

    out_selection = vec4(selection_value / 100.0f);
}
