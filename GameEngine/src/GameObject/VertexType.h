#pragma once
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
	// glm::vec3 color;
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