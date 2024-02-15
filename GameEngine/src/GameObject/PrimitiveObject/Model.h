#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../MeshMaterialsLight.h"
#include "BaseObject.h"

class Model : public BaseObject
{
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::string path;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string textureType);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
public:
	Model(const char* path);
	~Model();

	void Draw(const char* shaderProgramName);
};

