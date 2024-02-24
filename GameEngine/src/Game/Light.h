#pragma once
#include "../GameObject/PrimitiveObject/Box.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct LightPower {
	float constant;
	float linear;
	float quadratic;
};


class Light
{
private:
	static std::vector<LightPower> lightPowers;

public:
	shared_ptr<Box> box;
	Light();
	void Draw(const char* shaderProgramName);
	// 0 : directional , 1 : point, 2 : spot
	int lightType = 1;

	// 선택함.
	int lightPower = 3;

	// directional light, spot light 에 사용.
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

	float cutOff = glm::cos(glm::radians(12.5f));
	float cutOuter = glm::cos(glm::radians(17.5f));

	void PutLightUniform(const char* shaderProgramName, int lightPosition);

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

