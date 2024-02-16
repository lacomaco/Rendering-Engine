#pragma once
#include "./PrimitiveObject/Box.h"
#include <memory>

class Light
{
public:
	shared_ptr<Box> box;
	Light();
	void Draw(const char* shaderProgramName);
	glm::vec3 strength = glm::vec3(1.0f);
	float fallOffStart = 0.0f;
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
	float fallOffEnd = 10.0f;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f); // box position과 동일함.
	float spotPower = 0.0f;
	// 0 : directional , 1 : point, 2 : spot
	int lightType = 1;

	void PutLightUniform(const char* shaderProgramName);
};

