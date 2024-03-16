#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>
#include "lighting.h"
#include <string_view>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoordinates;
};

enum class TexMapType
{
	DIFFUSE, SPECULAR, EMISSIVE
};

struct TexMap
{
	unsigned int id;
	TexMapType type;
};

class Mesh
{
private :
	unsigned int vbo, ebo, vao;

public :
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TexMap> textures;

	Mesh();
	~Mesh();

	void updateMesh(bool verts, bool inds);
	void draw(unsigned int shader);
};

#endif