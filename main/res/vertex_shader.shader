#version 460 core

uniform mat4 u_modelTransform[1];
uniform mat4 u_view;
uniform mat4 u_projection;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 texCoord;

void main()
{
	gl_Position = u_projection * u_view * u_modelTransform[gl_InstanceID] * vec4(i_position, 1.0f);
	texCoord = i_texCoord;
}