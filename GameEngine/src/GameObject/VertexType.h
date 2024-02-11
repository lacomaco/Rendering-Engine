#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "../Util/Shader.h"
#include "assimp/aabb.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
	glm::vec3 tangentModel;

	// 값이 없을때 곱하기 연산시 이상이 없도록 1.0으로 초기화.
	glm::vec3 color = {
		1.0f, 1.0f, 1.0f 
	};
};

/*
* 텍스처이름들.
* albedoTexture; <- diffuseTexture일 경우 albedo로 이동시킴.
* emissiveTexture;
* normalTexture; <- bumTexture일 경우 이곳으로 이동
* heightTexture;
* metallicTexture;
* aoTexture;
* roughnessTexture;
* specularTexture
* 
* 
* 텍스처가 중복되는 경우 +1로 숫자를 붙여서 처리한다.
* 
* example)
* albedoTexture0
* albedoTexture1 ...
*/

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	/*
	Mesh(std::vector<Vertex>&& vertices,
		std::vector<unsigned int>&& indices);
	*/
	Mesh(std::vector<Vertex>&& _vertices, 
		std::vector<unsigned int>&& _indices,
		std::vector<Texture>&& _textures = {});
	~Mesh();

	void Draw();
	/*
	* assimp 사용해서 모델 긁어올 경우엔 사용하지 말고
	* Plane, Box, Sphere 같이 내가 생성하는 vertex의 Tangents를 사용하는
	* 경우에만 쓴다.
	*/
	void CalculateTangents();
	void setupMesh();


private:
	unsigned int VAO, VBO, EBO;
};

/*
* Material은 주로 
*/
struct Material {
	glm::vec3 ambient = glm::vec3(0.1f);
	float shininess = 1.0f;
	glm::vec3 diffuse = glm::vec3(0.5f);
	// TMI: DirectX에서는 constant buffer로 보내는 데이터 4byte 단위로 정렬하기 위해
	// 여기에 float dummy padding을 넣어야한다.
	glm::vec3 specular = glm::vec3(0.5f);
};

struct Light {
	glm::vec3 strength = glm::vec3(1.0f);
	float fallOffStart = 0.0f;

	// opengl은 right hand 좌표계를 쓰기 때문에 -z 방향이 앞쪽이다.
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
	float fallOffEnd = 10.0f;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
	float spotPower = 0.0f;
};