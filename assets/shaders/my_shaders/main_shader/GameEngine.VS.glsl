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

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
