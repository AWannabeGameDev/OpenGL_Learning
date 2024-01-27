#ifndef SQUARE_H
#define SQUARE_H

#include "prefab.h"

#include <glm/glm.hpp>

template<>
struct Vertex<SQUARE>
{
	glm::vec3 position;
	glm::vec2 textureCoordinate;
};

template<>
struct Prefab<SQUARE>
{
	Vertex<SQUARE> vertices[4];
	unsigned int indices[6];
};

template<>
void setVAOFormat<SQUARE>(unsigned int vaoID);
template<>
void attachVBO<SQUARE>(unsigned int vaoID, unsigned int vboID, size_t start);
template<>
void attachEBO<SQUARE>(unsigned int vaoID, unsigned int eboID);

template<>
void createPrefab<SQUARE>(Prefab<SQUARE>& prefab);

#endif