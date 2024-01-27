#include "cube.h"
#include <GL/glew.h>

template<>
void createPrefab<CUBE>(Prefab<CUBE>& prefab)
{
	static Vertex<CUBE> vertices[24] =
	{
		// face +x
		{{0.5f, 0.5f, 0.5f}}, // I quadrant
		{{0.5f, -0.5f, 0.5f}}, // II quadrant
		{{0.5f, -0.5f, -0.5f}}, // III quadrant
		{{0.5f, 0.5f, -0.5f}}, // IV quadrant

		// face -x
		{{-0.5f, 0.5f, 0.5f}},
		{{-0.5f, -0.5f, 0.5f}},
		{{-0.5f, -0.5f, -0.5f}},
		{{-0.5f, 0.5f, -0.5f}},

		// face +y
		{{0.5f, 0.5f, 0.5f}},
		{{-0.5f, 0.5f, 0.5f}},
		{{-0.5f, 0.5f, -0.5f}},
		{{0.5f, 0.5f, -0.5f}},

		// face -y
		{{0.5f, -0.5f, 0.5f}},
		{{-0.5f, -0.5f, 0.5f}},
		{{-0.5f, -0.5f, -0.5f}},
		{{0.5f, -0.5f, -0.5f}},

		// face +z
		{{0.5f, 0.5f, 0.5f}},
		{{-0.5f, 0.5f, 0.5f}},
		{{-0.5f, -0.5f, 0.5f}},
		{{0.5f, -0.5f, 0.5f}},

		// face -z
		{{0.5f, 0.5f, -0.5f}},
		{{-0.5f, 0.5f, -0.5f}},
		{{-0.5f, -0.5f, -0.5f}},
		{{0.5f, -0.5f, -0.5f}},
	};

	static unsigned int indices[36] =
	{
		// upper-left triangle     // lower-right triangle
		0+(4*0), 1+(4*0), 2+(4*0), 2+(4*0), 3+(4*0), 0+(4*0),
		0+(4*1), 1+(4*1), 2+(4*1), 2+(4*1), 3+(4*1), 0+(4*1),
		0+(4*2), 1+(4*2), 2+(4*2), 2+(4*2), 3+(4*2), 0+(4*2),
		0+(4*3), 1+(4*3), 2+(4*3), 2+(4*3), 3+(4*3), 0+(4*3),
		0+(4*4), 1+(4*4), 2+(4*4), 2+(4*4), 3+(4*4), 0+(4*4),
		0+(4*5), 1+(4*5), 2+(4*5), 2+(4*5), 3+(4*5), 0+(4*5),
	};

	std::copy(vertices, vertices + 24, prefab.vertices);
	std::copy(indices, indices + 36, prefab.indices);
}

template<>
void setVAOFormat<CUBE>(unsigned int vaoID)
{
	glBindVertexArray(vaoID);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex<CUBE>, position));
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex<CUBE>, textureCoordinate));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

template<>
void attachVBO<CUBE>(unsigned int vaoID, unsigned int vboID, size_t start)
{
	glBindVertexArray(vaoID);
	glBindVertexBuffer(0, vboID, start * sizeof(Vertex<CUBE>), sizeof(Vertex<CUBE>));
	glBindVertexArray(0);
}

template<>
void attachEBO<CUBE>(unsigned int vaoID, unsigned int eboID)
{
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}