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
        
uniform vec3 helicopter_position;

// GENERATED Uniforms
uniform float time;


// Node functions
float remap(float value, vec2 in_interval, vec2 out_interval)
{
    // Bring the in interval from [xi, yi] to [0, 1]
    value = (value - in_interval.x) / (in_interval.y - in_interval.x);
    
    // Bring the value from [0, 1] to [xo, yo]
    return value * (out_interval.y - out_interval.x) + out_interval.x;
}

// End node functions

vec3 get_light_contribution();
float get_fog_factor(float dist);

void main()
{
    vec3 light = get_light_contribution();

    // GENERATED Get the shader color
    vec4 shader_color = vec4(1, 1, 1, 1);
    shader_color = vec4(remap(sin(time), vec2(-1.000000f, 1.000000f), vec2(0.000000f, 1.000000f)), 1.000000f, 0.700000f, 1.000000f);

    // Apply light to color
    vec4 tex = texture2D(texture_1, tex_scale * tex_coord);
    if (tex.a * mesh_color.a * shader_color.a < 0.5)
        discard;

    // Light * texture * color
    vec4 lit_vertex = vec4(light * tex.xyz * mesh_color.xyz * shader_color.xyz, 1);

    // Apply fog calculations and output them
    // float alpha = (render_fog == true) ? get_fog_factor(distance(eye_position, world_position)) : 0.0;
    float alpha = get_fog_factor(distance(helicopter_position, world_position));
    out_color = mix(lit_vertex, fog_color, alpha);

    out_selection = vec4(selection_value / 100.0f);
}
