#version 330

// Input
in vec3 frag_color;

in vec3 world_position;
in vec3 world_normal;

in vec2 tex_coord;

in vec3 selection_color;

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

uniform float time;

uniform int is_shadow_pass;

uniform vec3 helicopter_position;

const float G_SCATTERING = -0.3f;

vec3 sample_normal_map(in sampler2D normal_texture, vec2 texture_coord);
vec3 get_light_contribution(vec3 normal);
float get_fog_factor(float dist);

// Mie scattering approximated with Henyey-Greenstein phase function
float compute_scattering(float cosAngle)
{
    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * 3.14 * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * cosAngle, 1.5f));
    return result;
}

vec4 get_texture(inout vec3 normal)
{
    const float water_level = 0.1f;
    const float water_transition_level = 0.115f;
    const float water_ground_level = 0.18f;
    const float ground_snow_level = 0.4f;
    const float snow_level = 0.7f;
    
    vec2 tex_coord_ground = tex_scale * tex_coord * 10;
    vec2 tex_coord_snow = tex_scale * tex_coord * 10;
    vec2 tex_coord_water = tex_scale * tex_coord * 5 + vec2(time * 0.005f, time * 0.01f);
    
    vec4 ground_tex = texture2D(texture_1, tex_coord_ground);
    float height = texture2D(texture_2, tex_scale * tex_coord).r;
    vec4 snow_tex = texture2D(texture_3, tex_coord_snow);
    vec4 water_tex = texture2D(texture_4, tex_coord_water);
    vec4 height_tex = vec4(vec3(height), 1);
    
    vec4 ground_height_mix = mix(height_tex, ground_tex, height / water_ground_level);
    
    if (height < water_level) {
        normal = world_normal;
        return water_tex;
    }
    
    if (height < water_transition_level) {
        float t = (height - water_level) / (water_transition_level - water_level);
        normal = world_normal;
        return mix(water_tex, ground_height_mix, t);
    }
    
    if (height < water_ground_level) {
        normal = sample_normal_map(normal_1, tex_coord_ground);
        return ground_height_mix;
    }
    
    if (height < ground_snow_level) {
        normal = sample_normal_map(normal_1, tex_coord_ground);
        return ground_tex;
    }
    
    if (height < snow_level) {
        float t = (height - ground_snow_level) / (snow_level - ground_snow_level);
        normal = mix(sample_normal_map(normal_1, tex_coord_ground), sample_normal_map(normal_3, tex_coord_snow), t);
        return mix(ground_tex, snow_tex, t);
    }
    
    normal = sample_normal_map(normal_3, tex_coord_snow);
    return snow_tex;
}

void main()
{
    // Apply light to color
    vec3 normal = vec3(0, 1, 0);
    vec4 tex = get_texture(normal);
    if (tex.a * mesh_color.a < 0.5)
        discard;

    if (use_normal_maps == 0) {
        normal = world_normal;
    }
    
    // Get light contribution
    vec3 light = get_light_contribution(normal);

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

