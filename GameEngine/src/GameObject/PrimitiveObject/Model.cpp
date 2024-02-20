#include "Model.h"
#include "../../Util/stb_image.h";
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

Model::Model(const char* path)
{
	this->path = path;
	loadModel(path);
	count = 0;
}

Model::~Model()
{
	for (auto& mesh : meshes) {

	}
}

void Model::Draw(const char* shaderProgramName)
{
	PutModelUniform(shaderProgramName);
	for (auto& mesh : meshes) {
		mesh->Draw(shaderProgramName);
	}
}

// see: https://learnopengl.com/Model-Loading/Model
void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	glm::mat4 tr = glm::mat4(1.0f);

	processNode(scene->mRootNode, scene, tr);
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 tr)
{
	glm::mat4 m = AiMatrix4x4ToGlmMat4(node->mTransformation);

	// volumatric god rays로 대체할예정..
	// sketchfab에서 받은 모델에 rodrays 큐브가 있는데 블렌더로 어떻게 지우는지 모르겠음....
	if (std::strcmp(node->mName.C_Str(),"GodRays_GodRays_0") == 0) {
		std::cout <<"cut god ray" << std::endl;
		return;
	}

	m = tr * m;

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto newMesh = processMesh(mesh, scene);

		for (auto& v : newMesh->vertices) {
			// 정점 위치를 변환
			glm::vec4 pos = glm::vec4(v.position.x, v.position.y, v.position.z, 1.0);
			pos = m * pos;
			v.position.x = pos.x;
			v.position.y = pos.y;
			v.position.z = pos.z;
		}

		meshes.push_back(newMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene,m);
	}
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		vertex.position = vector;

		if (mesh->HasNormals()) {
			glm::vec3 normal;

			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;

			vertex.normal = normal;
		}
		else {
			std::cout << path << " has no normals" << std::endl;
		}

		if (mesh->mTextureCoords[0]) {
			glm::vec2 texcoord;
			texcoord.x = mesh->mTextureCoords[0][i].x;
			texcoord.y = mesh->mTextureCoords[0][i].y;

			vertex.texcoord = texcoord;

			glm::vec3 tangent;
			tangent.x = mesh->mTangents[i].x;
			tangent.y = mesh->mTangents[i].y;
			tangent.z = mesh->mTangents[i].z;

			vertex.tangentModel = tangent;
		}
		else {
			std::cout << path << " has no texture coordinates" << std::endl;
		}



		vertices.push_back(vertex);
	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process material
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	/*
	* diffuse == albedo == baseColor
	*
	* 이름이 혼재되어있지만 같은 역할임.
	*/
	std::vector<Texture> albedoMaps = loadMaterialTextures(material,
		aiTextureType_DIFFUSE, "albedo");
	textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

	albedoMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "albedo");
	textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

	std::vector<Texture> specularMaps = loadMaterialTextures(material,
		aiTextureType_SPECULAR, "specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<Texture> normalMaps = loadMaterialTextures(material,
		aiTextureType_NORMALS, "normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<Texture> heightMaps = loadMaterialTextures(material,
		aiTextureType_HEIGHT, "height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	std::vector<Texture> emissiveMaps = loadMaterialTextures(material,
		aiTextureType_EMISSIVE, "emissive");
	textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

	std::vector<Texture> metallicMaps = loadMaterialTextures(material,
		aiTextureType_METALNESS, "metallic");
	textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

	std::vector<Texture> aoMaps = loadMaterialTextures(material,
		aiTextureType_AMBIENT_OCCLUSION, "ao");
	textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

	std::vector<Texture> roughnessMaps = loadMaterialTextures(material,
		aiTextureType_DIFFUSE_ROUGHNESS, "roughness");
	textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());


	auto ourMesh = std::make_shared<Mesh>(vertices, indices, textures);
	ourMesh->setupMesh();

	return ourMesh;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string textureType)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		auto fileName = ExtractFileName(str.C_Str());

		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), fileName.c_str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			int nrComponents;
			texture.id = TextureFromFile(fileName.c_str(), directory,nrComponents);
			texture.type = textureType;
			texture.path = fileName;
			texture.isAlpha = nrComponents == 4;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

// 나중에 텍스처 생성 옵션은 따로 컨트롤 가능하도록 해야함.
unsigned int Model::TextureFromFile(const char* path, const std::string& directory, int& nrComponents, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	std::cout << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if(nrComponents == 2)
			format = GL_RG;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


// aiMatrix4x4에서 glm::mat4로 변환하는 헬퍼 함수
glm::mat4 Model::AiMatrix4x4ToGlmMat4(const aiMatrix4x4& from) {
	glm::mat4 to;

	// aiMatrix4x4는 row-major, glm::mat4는 column-major
	to[0][0] = from.a1; to[1][0] = from.a2;
	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;
	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;
	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;
	to[2][3] = from.d3; to[3][3] = from.d4;

	return to;
}

std::string Model::ExtractFileName(const std::string& path)
{
	size_t pos = path.find_last_of("\\/");
	if (pos != std::string::npos) {
		return path.substr(pos + 1);
	}
	else {
		return path;
	}
}