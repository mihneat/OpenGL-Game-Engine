#version 430

// Input
layout(location = 0) in vec2 texture_coord;

in vec3 position;

// Uniform properties
uniform sampler2D texture_0;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;

uniform float light_space_near_plane;
uniform float light_space_far_plane;

// Output
layout(location = 0) out vec4 out_color;

#define zNear 0.001   // light_space_near_plane
#define zFar  1.0  // light_space_far_plane

void main()
{
    float depth = 0;
    depth = texture(texture_1, 1 - texture_coord).x;

    // This is useful for perspective projections, ortho works directly with depth :)
    // float lDepth = (zNear * zFar / (zFar + depth * (zNear - zFar))) / zFar;

    out_color = vec4(depth, depth, depth, 1);
}
