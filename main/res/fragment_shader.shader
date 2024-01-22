#version 460 core

uniform sampler2D u_sampler;
uniform float u_time;

in vec3 vertexColor;
in vec3 texCoord;

out vec4 color;

void main()
{
	color = texture(u_sampler, texCoord.xy + vec2(sin((texCoord.y * 2.5f) + (u_time / 2.0f)) / 2.0f, 0.0f));
}