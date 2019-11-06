#version 450

in vec3 position;
in vec3 color;

out vec3 color_out;

uniform mat4 scale;

void main()
{
    gl_Position = scale * vec4(position , 1.0);

	color_out = color;
}