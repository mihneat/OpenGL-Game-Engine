#version 330

// Input
in vec3 frag_color;

in vec3 world_position;
in vec3 world_normal;

in vec2 tex_coord;

in vec3 selection_color;

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

uniform float time;

uniform int is_shadow_pass;

uniform vec3 helicopter_position;

const float G_SCATTERING = -0.3f;

vec3 get_light_contribution();
float get_fog_factor(float dist);

// Mie scattering approximated with Henyey-Greenstein phase function
float compute_scattering(float cosAngle)
{
    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * 3.14 * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * cosAngle, 1.5f));
    return result;
}

vec4 get_texture()
{
    const float water_level = 0.1f;
    const float water_transition_level = 0.115f;
    const float water_ground_level = 0.18f;
    const float ground_snow_level = 0.4f;
    const float snow_level = 0.7f;
    
    vec4 ground_tex = texture2D(texture_1, tex_scale * tex_coord);
    float height = texture2D(texture_2, tex_scale * tex_coord).r;
    vec4 snow_tex = texture2D(texture_3, tex_scale * tex_coord);
    vec4 water_tex = texture2D(texture_4, tex_scale * tex_coord + vec2(time * 0.005f, time * 0.01f));
    vec4 height_tex = vec4(vec3(height), 1);
    
    vec4 ground_height_mix = mix(height_tex, ground_tex, height / water_ground_level);
    
    if (height < water_level)
        return water_tex;
    
    if (height < water_transition_level)
        return mix(water_tex, ground_height_mix, (height - water_level) / (water_transition_level - water_level));
    
    if (height < water_ground_level)
        return ground_height_mix;
    
    if (height < ground_snow_level)
        return ground_tex;
    
    if (height < snow_level)
        return mix(ground_tex, snow_tex, (height - ground_snow_level) / (snow_level - ground_snow_level));
    
    return snow_tex;
}

void main()
{
    vec3 light = get_light_contribution();

    // Apply light to color
    vec4 tex = get_texture();
    if (tex.a * mesh_color.a < 0.5)
        discard;

    // Light * texture * color
    vec4 lit_vertex = vec4(light * tex.xyz * mesh_color.xyz, 1);

    // Apply fog calculations and output them
    // float alpha = (render_fog == true) ? get_fog_factor(distance(eye_position, world_position)) : 0.0;
    float alpha = get_fog_factor(distance(helicopter_position, world_position));
    out_color = mix(lit_vertex, fog_color, alpha);
    
    out_selection = vec4(selection_color, 0);

    // This is for the ortho camera to generate a depth map
    //    if (is_shadow_pass)
    //        gl_FragDepth = world_position.z;
}

