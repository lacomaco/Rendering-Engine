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
	// 0 : directional , 1 : point, 2 : spot
	int lightType = 1;

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;


	glm::vec3 ambient = glm::vec3(0.2f);
	glm::vec3 diffuse = glm::vec3(0.8f);
	glm::vec3 specular = glm::vec3(1.0f);

	// directional light, spot light ¿¡ »ç¿ë.
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

