#pragma once
#include "./PrimitiveObject/Box.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


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
	float spotPower = 0.0f; // 스포트라이트 빛에 사용.
	// 0 : directional , 1 : point, 2 : spot
	int lightType = 1;

	float specularStrength = 0.5f;

	void PutLightUniform(const char* shaderProgramName);

	glm::vec3 getPosition() {
		return box->position;
	}

	void setPosition(glm::vec3 position) {
		box->position = position;
	}

	glm::quat yRotationQuat = glm::angleAxis(
		glm::radians(0.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	void Update(float deltaTime) {
		yRotationQuat =
			glm::angleAxis(glm::radians(90.0f) * deltaTime,glm::vec3(0,1,0));

		glm::mat4 rotationMatrix = glm::toMat4(yRotationQuat);

		glm::vec3 rotatedPosition = glm::vec3(
			rotationMatrix * glm::vec4(box->position, 1.0f)
		);

		setPosition(rotatedPosition);

	}
};

