#version 460 core

uniform sampler2D u_sampler;

in vec2 texCoord;

out vec4 color;

void main()
{
	color = texture(u_sampler, texCoord);
}