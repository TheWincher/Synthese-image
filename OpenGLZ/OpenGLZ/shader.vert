#version 450

in vec3 position;
in vec2 uv;
in vec3 color;
//in float speed;

uniform mat4 view, modelView, projection;

out vec3 color_out;
out vec2 uv_out;

void main()
{
    gl_Position = projection * view * modelView * vec4(position , 1.0);

	color_out = color;

	//Pour les textures
	//uv_out = uv;
}