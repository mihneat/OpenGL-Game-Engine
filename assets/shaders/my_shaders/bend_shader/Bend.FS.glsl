#version 330

// Input
in vec3 frag_color;

in vec3 world_position;
in vec3 world_normal;

in vec2 tex_coord;

// Output
layout(location = 0) out vec4 out_color;

// Uniform 
uniform vec3  eye_position;
uniform vec3  light_positions[10];
uniform vec3  light_directions[10];
uniform int   light_types[10];
uniform float light_intensities[10];
uniform float time_of_day;

uniform vec4  fog_color;
uniform bool  render_fog;

uniform sampler2D texture_bait;
uniform sampler2D texture_color;
uniform sampler2D texture_normal;

float sun_light_contribution(vec3 light_pos, float intensity)
{
    float material_kd = 0.4;

    vec3 n_world_normal = normalize( mix( world_normal, vec3(texture2D(texture_normal, tex_coord)) / 256, 0.5) );

    vec3 L = normalize( light_pos );
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );

    float global_ambiental_color = 0.3;
    float ambient_light = material_kd * global_ambiental_color;
    
    float diffuse_light = material_kd * max (dot(n_world_normal, L), 0);

    float specular_light = 0.0;

    float material_ks = 0.3;
    float material_shininess = 0.8;

    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(n_world_normal, H), 0), material_shininess);
    }

    // Use an attenuation factor of 1 (casting a sun ray, thus no attenuation)
    // Actually, attenuate based on time of day
    float light = intensity * time_of_day * ( diffuse_light + specular_light );

	return light;
}

float spot_light_contribution(vec3 light_pos, vec3 light_dir, float intensity)
{
    float material_kd = 0.4;

    vec3 n_world_normal = normalize( world_normal );

    vec3 L = normalize( light_pos - world_position );
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );

    float global_ambiental_color = 0.3;
    float ambient_light = material_kd * global_ambiental_color;
    
    float diffuse_light = material_kd * max (dot(n_world_normal, L), 0);

    float specular_light = 0.0;

    float material_ks = 0.3;
    float material_shininess = 0.8;

    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(n_world_normal, H), 0), material_shininess);
    }

    // Apply the spot light attenuation
    float cut_off = radians(30.0);
    float spot_light = dot(-L, normalize(light_dir));
    float spot_light_limit = cos(cut_off);

    float light_att_factor = 0.0f;

    if (spot_light > spot_light_limit)
    {
        float linear_att = (spot_light - spot_light_limit) / (1.0 - spot_light_limit);
        light_att_factor = pow(linear_att, 2);
    }

    float attenuation_factor = 1 / (200 + 0.4 * (distance(light_pos, world_position)) * (distance(light_pos, world_position)));
    float light = intensity * attenuation_factor * light_att_factor * ( diffuse_light + specular_light );

	return light;
}

float get_fog_factor(float dist)
{
    const float fog_max = 500.0;
    const float fog_min = 20.0;

    if (dist >= fog_max) return 1;
    if (dist <= fog_min) return 0;

    return 1 - (fog_max - dist) / (fog_max - fog_min);
}

void main()
{
    float light = 0;

    for (int i = 0; i < 3; ++i) {
        if (light_types[i] == 0) {
            light += sun_light_contribution(light_positions[i], light_intensities[i]);
        } else if (light_types[i] == 1) {
            light += spot_light_contribution(light_positions[i], light_directions[i], light_intensities[i]);
        }
    }
    
    float material_kd = 0.9;
    float global_ambiental_color = 0.9;
    float ambient_light = material_kd * global_ambiental_color;
    light += max(time_of_day, 0.4) * ambient_light;

    // Apply light to color
    vec4 lit_vertex = vec4(light * frag_color, 1);
    // lit_vertex = light * texture2D(texture_normal, tex_coord);
    // lit_vertex = light * texture2D(texture_color, tex_coord);

    // Apply fog calculations and output them
    float alpha = (render_fog == true) ? get_fog_factor(distance(eye_position, world_position)) : 0.0;
    out_color = mix(lit_vertex, fog_color, alpha);
}

