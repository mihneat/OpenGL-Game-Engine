#version 330

// Structure definitions
struct light_source {
    int isUsed;
    int type;
    float intensity;
    vec3 position;
    vec3 color;
    vec3 direction;
};

// Input
in vec3 frag_color;

in vec3 world_position;
in vec3 world_normal;

in vec2 tex_coord;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_selection;

// Uniform 
uniform vec3 eye_position;
uniform light_source lights[100]; // Max lights also need to be changed in LightManager.h
// Also modify maximum 'for' value below

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

uniform vec4 mesh_color;

uniform float selection_value;

void main()
{
    // Apply light to color
    vec4 tex = texture2D(texture_1, tex_scale * tex_coord);
    if (tex.a * mesh_color.a < 0.5)
        discard;

    // Unlit: Texture * color
    out_color = vec4(tex.xyz * mesh_color.xyz, 1);

    out_selection = vec4(0, 0, 0, -1);
}
