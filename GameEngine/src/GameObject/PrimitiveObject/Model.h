#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../MeshMaterialsLight.h"
#include "BaseObject.h"
#include <memory>
#include <thread>
#include <filesystem>
#include <iostream>
#include <mutex>

struct RawTextureData {
	std::string fileName;
	unsigned char* data;
	int width;
	int height;
	int nrComponents;
	GLenum internalFormat;
	GLenum dataFormat;
	bool hasAlpha;
};

class Model : public BaseObject
{
private:
	std::string directory;
	std::string path;
	bool gammaCorrection;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 tr);
	std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 tr, std::string nodeName);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string textureType);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool& hasAlpha, bool gamma = false);
	std::string ExtractFileName(const std::string& path);


	// 텍스처 로드 관련.
	bool IsImageFIle(const std::string& fileName);
	void LoadTexture(
		const std::string& fileName,
		const std::string& directory
	);
	void PreLoadTextures(std::string directory);
	void FreePreLoadTextures();

	RawTextureData FindTextureData(const std::string& fileName);

	std::mutex mtx;
	std::vector<RawTextureData> texturesData;

public:
	std::vector<std::shared_ptr<Mesh>> meshes;
	Model(const char* path);
	~Model();


	void Draw(const char* shaderProgramName);
	void SetScale(glm::vec3 _scale);
};

