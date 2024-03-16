#include <utility/mesh.h>
#include <GL/glew.h>
#include <utility/util.h>
#include <string>

Mesh::Mesh()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, textureCoordinates));
	glVertexAttribBinding(2, 0);
	glEnableVertexAttribArray(2);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vao);
}

void Mesh::updateMesh(bool verts, bool inds)
{
	glBindVertexArray(vao);

	if(verts)
	{
		glDeleteBuffers(1, &vbo);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));
	}

	if(inds)
	{
		glDeleteBuffers(1, &ebo);
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	}
}

void Mesh::draw(unsigned int shader)
{
	glBindVertexArray(vao);
	glUseProgram(shader);
	
	int diffuseIndex = 0, specularIndex = 0, emissiveIndex = 0;
	for(int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);

		if(textures[i].type == TexMapType::DIFFUSE)
			setUniform(shader, "diffuse" + std::to_string(diffuseIndex++), i);
		else if(textures[i].type == TexMapType::SPECULAR)
			setUniform(shader, "specular" + std::to_string(specularIndex++), i);
		else if(textures[i].type == TexMapType::EMISSIVE)
			setUniform(shader, "emissive" + std::to_string(emissiveIndex++), i);
	}

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (const void*)0);
}