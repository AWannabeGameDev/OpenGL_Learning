#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct Material
{
	glm::vec4 ambientColor;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
	float shininess;
};

struct LightSource
{
	glm::vec3 position;
	glm::vec4 ambientColor;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
};

#endif