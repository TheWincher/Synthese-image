#version 450

in vec3 color_out;
in vec2 uv_out;

out vec4 color;

uniform sampler2D texScreen, texFrameBuffer;

void main()
{
    color = texture(texScreen, uv_out);
	//color = vec4(uv_out, 0,1);
}