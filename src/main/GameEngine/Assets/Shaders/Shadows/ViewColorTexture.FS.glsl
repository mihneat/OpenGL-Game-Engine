#version 430

// Input
layout(location = 0) in vec2 texture_coord;

in vec3 position;

// Uniform properties
uniform sampler2D texture_0;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec4 color = vec4(1, 0, 0, 1);
    if (position.x < 0 && position.y > 0)
        color = texture(texture_0, (1 - texture_coord) * 2);
    else if (position.x > 0 && position.y > 0)
        color = texture(texture_1, (1 - texture_coord) * 2);
    else if (position.x < 0 && position.y < 0)
        color = texture(texture_2, (1 - texture_coord) * 2);
    else if (position.x > 0 && position.y < 0)
        color = texture(texture_3, (1 - texture_coord) * 2);

    out_color = color;
}
