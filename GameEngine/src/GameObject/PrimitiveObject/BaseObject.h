#pragma once
#include <glm/glm.hpp>
#include "../MeshMaterialsLight.h"

class BaseObject {
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	Material material;

	void PutModelUniform(const char* shaderProgramName);
};