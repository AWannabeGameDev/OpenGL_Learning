#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <stb_image/load.h>

struct Material
{
	TextureData diffuseMap;
	TextureData specularMap;
	float shininess;
	TextureData emissionMap;
	glm::vec4 emissionColor;
};

struct PointLight
{
	glm::vec3 position;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
	float attenConst;
	float attenLinear;
	float attenQuad;
};

struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
};

struct ConeLight
{
	glm::vec3 direction;
	glm::vec3 position;
	float angle;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
};

#endif