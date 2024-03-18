#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include <vector>
#include <string_view>
#include <assimp/scene.h>
#include <unordered_map>

class Model
{
private :
	std::vector<Mesh> meshes;
	std::unordered_map<std::string, unsigned int> loadedTextures;

	void loadMeshesFromNode(const aiNode* node, const aiScene* scene);
	void loadTexturesFromMaterial(const aiMaterial* material, aiTextureType aiType, TexMapType type, 
								  std::vector<TexMap>& textureList);

public :
	Model(std::string_view path);
	~Model();

	friend void swap(Model& first, Model& second);
	Model(const Model& other);
	Model(Model&& other) noexcept;
	Model& operator=(Model other);

	void draw(unsigned int shader);
	void setTextures(unsigned int shader);
};

void swap(Model& first, Model& second);

#endif