#version 460 core

layout(location = 0) in	vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_texCoord;

uniform mat4 u_model;
uniform mat3 u_normalMatrix;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 fragWorldCoord;
out vec3 fragWorldNormal;
out vec2 fragTexCoord;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0f);
	fragWorldCoord = vec3(u_model * vec4(i_position, 1.0f));
	fragWorldNormal = u_normalMatrix * i_normal;
	fragTexCoord = i_texCoord;
}