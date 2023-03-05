#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 car_position;
uniform float scale_factor;

uniform int distort_mode;

// Output
out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;

out vec3 world_position;
out vec3 world_normal;


void main()
{
    world_position = (Model * vec4(v_position,1)).xyz;
    world_normal = normalize( mat3(Model) * v_normal );

    frag_normal = v_normal;
    frag_color = v_color;
    tex_coord = v_texture_coord;

    vec4 modeled_pos = Model * vec4(v_position, 1.0);

    float dist = 0.0;
    if (distort_mode == 0) {
        dist = distance(car_position, vec3(Model * vec4(v_position, 1.0)));
        modeled_pos.y = modeled_pos.y - dist * dist * scale_factor;
    } else if (distort_mode == 1) {
        dist = distance(car_position, vec3(Model * vec4(v_position, 1.0)));
        modeled_pos.y = modeled_pos.y - sin(dist * dist * scale_factor);
    } else if (distort_mode == 2) {
        dist = distance(car_position, vec3(Model * vec4(v_position, 1.0)));
        modeled_pos.y = modeled_pos.y - 700 * scale_factor * sin(dist);
    } else if (distort_mode == 3) {
        dist = distance(car_position, vec3(Model * vec4(v_position, 1.0)));
        modeled_pos.y = modeled_pos.y - 50.0 * scale_factor * dist * sin(dist / 50);
    }

    gl_Position = Projection * View * modeled_pos;
}
