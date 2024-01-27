#ifndef CUBE_H
#define CUBE_H

#include "prefab.h"

#include <glm/glm.hpp>

template<>
struct Vertex<CUBE>
{
	glm::vec3 position;
	glm::vec2 textureCoordinate;
};

template<>
struct Prefab<CUBE>
{
	union
	{
		Vertex<CUBE> vertices[24];
		Vertex<CUBE> faces[6][4];
	};
	unsigned int indices[36];
};

template<>
void setVAOFormat<CUBE>(unsigned int vaoID);
template<>
void attachVBO<CUBE>(unsigned int vaoID, unsigned int vboID, size_t start);
template<>
void attachEBO<CUBE>(unsigned int vaoID, unsigned int eboID);

template<>
void createPrefab<CUBE>(Prefab<CUBE>& prefab);

#endif