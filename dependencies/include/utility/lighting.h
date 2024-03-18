#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "load_image.h"

struct Material
{
	unsigned int diffuseMapID;
	unsigned int specularMapID;
	float shininess;
	unsigned int emissionMapID;
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
	float angleCosine;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
	float attenConst;
	float attenLinear;
	float attenQuad;
};

#endif