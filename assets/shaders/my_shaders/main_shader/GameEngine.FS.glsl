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

// Uniform 
uniform vec3 eye_position;
uniform light_source lights[100]; // Max lights also need to be changed in LightManager.h
                                  // Also modify maximum 'for' value below

//uniform vec4  fog_color;
//uniform bool  render_fog;

uniform int is_scrolling;
uniform vec2 scroll_amount;

uniform int use_texture;
uniform vec2 tex_scale;
uniform sampler2D texture_color;
//uniform sampler2D texture_normal;

float sun_light_contribution(light_source light)
{
    float material_kd = 0.4;
    
    // vec3 n_world_normal = normalize( mix( world_normal, vec3(texture2D(texture_normal, tex_coord)) / 256, 0.5) );
    vec3 n_world_normal = world_normal;

    vec3 L = normalize( -light.direction );
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );

    float global_ambiental_color = 0.3;
    float ambient_light = material_kd * global_ambiental_color;
    
    float diffuse_light = material_kd * max(dot(n_world_normal, L), 0);

    float specular_light = 0.0;

    float material_ks = 0.3;
    float material_shininess = 0.8;

    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(n_world_normal, H), 0), material_shininess);
    }

    // Use an attenuation factor of 1 (casting a sun ray, thus no attenuation)
    // Actually, attenuate based on time of day
    float lightValue = light.intensity * (diffuse_light + specular_light);  // float light = intensity * ( diffuse_light + specular_light );

	return lightValue;
}

float spot_light_contribution(light_source light)
{
    float material_kd = 0.4;

    vec3 n_world_normal = normalize( world_normal );

    vec3 L = normalize( light.position - world_position );
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
    float cut_off = radians(45.0);
    float spot_light = dot(-L, normalize(light.direction));
    float spot_light_limit = cos(cut_off);

    float light_att_factor = 0.0f;

    if (spot_light > spot_light_limit)
    {
        float linear_att = (spot_light - spot_light_limit) / (1.0 - spot_light_limit);
        light_att_factor = pow(linear_att, 2);
    }

    float attenuation_factor = 1 / (200 + 0.4 * (distance(light.position, world_position)) * (distance(light.position, world_position)));
    float lightValue = light.intensity * attenuation_factor * light_att_factor * ( diffuse_light + specular_light );

	return lightValue;
}

float point_light_contribution(light_source light)
{
    float material_kd = 0.4;

    vec3 n_world_normal = normalize( world_normal );

    vec3 L = normalize( light.position - world_position );
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
    
    float attenuation_factor = 1 / (1 + (distance(light.position, world_position)) * (distance(light.position, world_position)));
    float lightValue = light.intensity * attenuation_factor * ( diffuse_light + specular_light );

	return lightValue;
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
    vec3 light = vec3(0.0);

    for (int i = 0; i < 100; ++i) {   // Also modify vector uniform
        if (lights[i].isUsed == 0) {
            continue;
        }

        if (lights[i].type == 0) {
            light += sun_light_contribution(lights[i]) * lights[i].color;
        } else if (lights[i].type == 1) {
            light += spot_light_contribution(lights[i]) * lights[i].color;
        } else if (lights[i].type == 2) {
            light += point_light_contribution(lights[i]) * lights[i].color;
        }
    }
    
    float material_kd = 0.9;
    float global_ambiental_color = 0.9;
    float ambient_light = material_kd * global_ambiental_color;
    light += 0.4 * ambient_light;  // light += max(time_of_day, 0.4) * ambient_light;

    // Apply light to color
    vec4 lit_vertex;
    if (use_texture == 0) {
        lit_vertex = vec4(light.x * frag_color.x, light.y * frag_color.y, light.z * frag_color.z, 1);
    } else {
        vec2 tex_offset = vec2(0);
        if (is_scrolling == 1) {
            tex_offset = scroll_amount;
        }

        // Ground size
        vec2 ground_size = vec2(1.0 / 200.0, 1.0 / (200 * sqrt(3) / 2.0));

        vec4 tex = texture2D(texture_color, tex_scale * vec2(tex_coord.x - tex_offset.x * ground_size.x, tex_coord.y - tex_offset.y * ground_size.y));
        if (tex.a < 0.5) {
            discard;
        }

        lit_vertex = vec4(light.x * tex.x, light.y * tex.y, light.z * tex.z, 1);
    }
    // lit_vertex = light * texture2D(texture_color, tex_coord);

    // Apply fog calculations and output them
    // float alpha = (render_fog == true) ? get_fog_factor(distance(eye_position, world_position)) : 0.0;
    out_color = lit_vertex;  // mix(lit_vertex, fog_color, alpha);
}

