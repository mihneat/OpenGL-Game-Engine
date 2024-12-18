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

in vec3 selection_color;

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
uniform sampler2D depth_texture;
//uniform sampler2D texture_normal;

uniform vec4 mesh_color;

uniform float time;

uniform mat4 light_space_view;
uniform mat4 light_space_projection;

uniform int is_shadow_pass;

uniform vec3 helicopter_position;

const float G_SCATTERING = -0.3f;

// Mie scattering approximated with Henyey-Greenstein phase function
float compute_scattering(float cosAngle)
{
    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * 3.14 * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * cosAngle, 1.5f));
    return result;
}

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

    float attenuation_factor = 1 / (1 + (distance(light.position, world_position)) * (distance(light.position, world_position)));
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
    const float fog_max = 200.0;
    const float fog_min = 20.0;

    if (dist >= fog_max) return 1;
    if (dist <= fog_min) return 0;

    return 1 - (fog_max - dist) / (fog_max - fog_min);
}

bool is_illuminated(vec3 point_position, float bias)
{
    vec4 light_space_pos = light_space_projection * light_space_view * vec4 (point_position, 1.0f);

    light_space_pos = light_space_pos / light_space_pos.w;

    float light_space_depth = light_space_pos.z * 0.5f + 0.5f;

    vec2 depth_map_pos = light_space_pos.xy * 0.5f + 0.5f;

    bvec2 a = greaterThan(depth_map_pos, vec2(1.0, 1.0));
    bvec2 b = lessThan(depth_map_pos, vec2(0.0, 0.0));

    if (any(bvec2(any(a), any(b)))) {
        return false;
    }

    float depth = texture(depth_texture, depth_map_pos).x;

    return light_space_depth - bias < depth;
}

float shadow_factor()
{
    return is_illuminated(world_position, 0.01f) ? 1.0f : 0.0f;
}

float volumetric_illumination(vec3 light_direction)
{
    vec3 ray_direction = world_position - eye_position;

    int illuminated_samples_count = 0;
    int sample_count = 0;

    vec3 point_position = eye_position;
    // TODO(student): Sample several hundreds of points between the
    // observer's coordinate and the world-space coordinate of the
    // point rendered in the current fragment. Quantify the number of
    // sampled points that are illuminated by the spot light source,
    // IsIlluminated() and the number of sampled points in total.
    for (int i = 0; i < 100; ++i) {
        vec3 sample_position = mix(point_position, world_position, i * 1.0f / 99.9f); // eye_position + ray_direction * i;

        if (is_illuminated(sample_position, 0.02f))
        illuminated_samples_count++;

        sample_count++;
    }

    float scattering = compute_scattering(dot(normalize(ray_direction), light_direction));

    return scattering * illuminated_samples_count / sample_count * 5;
}

vec3 get_light_contribution()
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
    
    light *= shadow_factor();

    float material_kd = 0.9;
    float global_ambiental_color = 0.9;
    float ambient_light = material_kd * global_ambiental_color;
    light += 0.4 * ambient_light;  // light += max(time_of_day, 0.4) * ambient_light;
    
    return light;
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

