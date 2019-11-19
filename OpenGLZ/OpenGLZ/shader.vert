#version 450

in vec3 position;
in vec2 uv;
in vec3 color;

out vec3 color_out;
out vec2 uv_out;

uniform mat4 scale;

void main()
{
    gl_Position = scale * vec4(position , 1.0);

	color_out = color;
	uv_out = uv;
}