#include "square.h"
#include <GL/glew.h>

template<>
void createPrefab<SQUARE>(Prefab<SQUARE>& prefab)
{
	static Vertex<SQUARE> vertices[4] =
	{	
		{{0.5f, 0.5f, 0.0f}},
		{{-0.5f, 0.5f, 0.0f}},
		{{-0.5f, -0.5f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}},
	};

	static unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

	std::copy(vertices, vertices + 4, prefab.vertices);
	std::copy(indices, indices + 6, prefab.indices);
}

template<>
void setVAOFormat<SQUARE>(unsigned int vaoID)
{
	glBindVertexArray(vaoID);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex<SQUARE>, position));
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex<SQUARE>, textureCoordinate));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

template<>
void attachVBO<SQUARE>(unsigned int vaoID, unsigned int vboID, size_t start)
{
	glBindVertexArray(vaoID);
	glBindVertexBuffer(0, vboID, start * sizeof(Vertex<SQUARE>), sizeof(Vertex<SQUARE>));
	glBindVertexArray(0);
}

template<>
void attachEBO<SQUARE>(unsigned int vaoID, unsigned int eboID)
{
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}