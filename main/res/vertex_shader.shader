#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_vertexColor;
layout(location = 2) in vec3 i_texCoord;

out vec3 vertexColor;
out vec3 texCoord;

void main()
{
	gl_Position = vec4(i_position, 1.0f);
	vertexColor = i_vertexColor;
	texCoord = i_texCoord;
}