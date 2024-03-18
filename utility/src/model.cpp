#include <utility/model.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stdio.h>
#include <utility/util.h>

namespace ai = Assimp;

void Model::loadMeshesFromNode(const aiNode* node, const aiScene* scene)
{
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		const aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
		Mesh& mesh = meshes.emplace_back();

		for(unsigned int i = 0; i < aiMesh->mNumVertices; i++)
		{
			Vertex& vertex = mesh.vertices.emplace_back();
			vertex.position.x = aiMesh->mVertices[i].x;
			vertex.position.y = aiMesh->mVertices[i].y;
			vertex.position.z = aiMesh->mVertices[i].z;
			vertex.normal.x = aiMesh->mNormals[i].x;
			vertex.normal.y = aiMesh->mNormals[i].y;
			vertex.normal.z = aiMesh->mNormals[i].z;

			if(aiMesh->mTextureCoords[0])
			{
				vertex.textureCoordinates.x = aiMesh->mTextureCoords[0][i].x;
				vertex.textureCoordinates.y = aiMesh->mTextureCoords[0][i].y;
			}
		}

		for(unsigned int i = 0; i < aiMesh->mNumFaces; i++)
		{
			aiFace face = aiMesh->mFaces[i];
			for(unsigned int j = 0; j < face.mNumIndices; j++)
				mesh.indices.push_back(face.mIndices[j]);
		}

		if(aiMesh->mMaterialIndex >= 0)
		{
			const aiMaterial* mat = scene->mMaterials[aiMesh->mMaterialIndex];
			loadTexturesFromMaterial(mat, aiTextureType_DIFFUSE, TexMapType::DIFFUSE, mesh.textures);
			loadTexturesFromMaterial(mat, aiTextureType_SPECULAR, TexMapType::SPECULAR, mesh.textures);
			loadTexturesFromMaterial(mat, aiTextureType_EMISSIVE, TexMapType::EMISSIVE, mesh.textures);
		}

		mesh.updateMesh(true, true);
	}

	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		loadMeshesFromNode(node->mChildren[i], scene);
	}
}

void Model::loadTexturesFromMaterial(const aiMaterial* material, aiTextureType aiType, TexMapType type, 
									 std::vector<TexMap>& textureList)
{
	for(unsigned int i = 0; i < material->GetTextureCount(aiType); i++)
	{
		TexMap& texture = textureList.emplace_back();
		texture.type = type;

		aiString texturePath;
		material->GetTexture(aiType, i, &texturePath);
		std::string texturePathStr{std::string{"../res/backpack/"} + texturePath.C_Str()};

		if(!loadedTextures.contains(texturePathStr))
		{
			TextureData data = loadTexture(texturePathStr);
			GLenum format;
			if(data.numChannels == 3)
				format = GL_RGB;
			else
				format = GL_RGBA;

			glGenTextures(1, &(texture.id));
			glBindTexture(GL_TEXTURE_2D, texture.id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.data);

			loadedTextures[texturePathStr] = texture.id;
		}
		else
		{
			texture.id = loadedTextures[texturePathStr];
		}
	}
}

Model::Model(std::string_view path)
{
	ai::Importer importer;
	const aiScene* scene = importer.ReadFile(std::string{path}, aiProcess_Triangulate | aiProcess_FlipUVs);

	if((scene == nullptr) || ((scene->mFlags != 0) && AI_SCENE_FLAGS_INCOMPLETE) || (scene->mRootNode == nullptr))
	{
		printf("Assimp model loading error : %s\n", importer.GetErrorString());	
		return;
	}

	loadMeshesFromNode(scene->mRootNode, scene);
}

Model::~Model()
{
	for(auto& [_, textureID] : loadedTextures)
	{
		glDeleteTextures(1, &textureID);
	}
}

void Model::draw(unsigned int shader)
{
	for(Mesh& mesh : meshes)
		mesh.draw(shader);
}

void Model::setTextures(unsigned int shader)
{
	for(Mesh& mesh : meshes)
		mesh.setTextures(shader);
}