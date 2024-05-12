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
	// TMI: DirectX������ constant buffer�� ������ ������ 4byte ������ �����ϱ� ����
	// ���⿡ float dummy padding�� �־���Ѵ�.
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
	// ��� vertex���� ��� ��ġ.
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
	* assimp ����ؼ� �� �ܾ�� ��쿣 ������� ����
	* Plane, Box, Sphere ���� ���� �����ϴ� vertex�� Tangents�� ����ϴ�
	* ��쿡�� ����.
	*/
	void CalculateTangents();
	void setupMesh();
	void CalculateVertexAveragePosition(glm::vec3 scale = glm::vec3(1.0f));



private:
	unsigned int VAO, VBO, EBO;
};
