#version 460 core

uniform mat4 u_positionTransform;
uniform mat4 u_texTransform;

layout(location = 0) in vec4 i_position;
layout(location = 1) in vec4 i_vertexColor;
layout(location = 2) in vec4 i_texCoord;

out vec4 vertexColor;
out vec4 texCoord;

void main()
{
	gl_Position = u_positionTransform * i_position;
	vertexColor = i_vertexColor;
	texCoord = u_texTransform * i_texCoord;
}