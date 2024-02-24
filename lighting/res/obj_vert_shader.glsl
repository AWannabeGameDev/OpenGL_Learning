#version 460 core

layout(location = 0) in	vec3 i_position;
layout(location = 1) in vec3 i_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 fragWorldCoord;
out vec3 fragWorldNormal;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0f);
	fragWorldCoord = vec3(u_model * vec4(i_position, 1.0f));
	fragWorldNormal = i_normal;
}