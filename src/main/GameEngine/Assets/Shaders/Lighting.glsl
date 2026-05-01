// Helper file containing lighting calculations
// Note: This file should be included AFTER the main one(s)

// Structure definitions
struct light_source {
    int isUsed;
    int type;
    float intensity;
    vec3 position;
    vec3 color;
    vec3 direction;
};

// Light uniforms
uniform light_source lights[100]; // Max lights also need to be changed in LightManager.h
// Also modify maximum 'for' value below

uniform sampler2D depth_texture;

uniform mat4 light_space_view;
uniform mat4 light_space_projection;

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

float shadow_factor(vec3 point_position)
{
    vec4 light_space_pos = light_space_projection * light_space_view * vec4 (point_position, 1.0f);

    light_space_pos = light_space_pos / light_space_pos.w;

    float light_space_depth = light_space_pos.z * 0.5f + 0.5f;

    vec2 depth_map_pos = light_space_pos.xy * 0.5f + 0.5f;

    bvec2 a = greaterThan(depth_map_pos, vec2(1.0, 1.0));
    bvec2 b = lessThan(depth_map_pos, vec2(0.0, 0.0));

    if (any(bvec2(any(a), any(b)))) {
        return 1.0f;
    }

    // return texture(depth_texture, vec3(depth_map_pos, light_space_depth));

    // OLD Approach using hard shadows
    const float bias = 0.01f;
    float depth = texture(depth_texture, depth_map_pos).x;
    bool is_illuminated = light_space_depth - bias < depth;
    return is_illuminated ? 1.0f : 0.0f;
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

    light *= shadow_factor(world_position);

    float material_kd = 0.9;
    float global_ambiental_color = 0.9;
    float ambient_light = material_kd * global_ambiental_color;
    light += 0.4 * ambient_light;  // light += max(time_of_day, 0.4) * ambient_light;

    return light;
}
