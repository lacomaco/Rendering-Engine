#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "../Util/Shader.h"
#include "assimp/aabb.h"
#include "FresnelIRO.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
	glm::vec3 tangentModel;
	glm::vec3 bitangentModel;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
	bool isAlpha = false;
	std::string signature; // path+type
};

extern std::vector<Texture> textures_loaded;

class Material {
public:
	float shininess = 32.0f;
	// TMI: DirectX에서는 constant buffer로 보내는 데이터 4byte 단위로 정렬하기 위해
	// 여기에 float dummy padding을 넣어야한다.
	glm::vec3 ambient = glm::vec3(0.1f);
	glm::vec3 diffuse = glm::vec3(1.0f);
	glm::vec3 specular = glm::vec3(1.0f);

	FresnelIRO type = FresnelIRO::DEFAULT;

	void PutMaterialUniforms(const char* shaderProgramName);
};


class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	// 모든 vertex들의 평균 위치.
	glm::vec3 position = glm::vec3(0.0f);
	bool isAlphaMesh = false;
	Material mat;
	/*
	Mesh(std::vector<Vertex>&& vertices,
		std::vector<unsigned int>&& indices);
	*/
	Mesh(std::vector<Vertex> _vertices,
		std::vector<unsigned int> _indices,
		std::vector<Texture>_textures = {});
	~Mesh();

	void Draw(const char* shaderProgramName);
	/*
	* assimp 사용해서 모델 긁어올 경우엔 사용하지 말고
	* Plane, Box, Sphere 같이 내가 생성하는 vertex의 Tangents를 사용하는
	* 경우에만 쓴다.
	*/
	void CalculateTangents();
	void setupMesh();
	void CalculateVertexAveragePosition(glm::vec3 scale = glm::vec3(1.0f));



private:
	unsigned int VAO, VBO, EBO;
};
