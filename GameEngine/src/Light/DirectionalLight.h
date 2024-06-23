#pragma once
#include "glm/glm.hpp"
#include "LightType.h"
#include "CascadeShadow.h"
#include <memory>
#include "../Game/MeshRenderer.h"
#include "./LightType.h"

using namespace glm;

class DirectionalLight
{
private:
	Light lightInfo;
	shared_ptr<CascadeShadow> Shadow;
	int cascadeLevel = 5.0f;

public:
	DirectionalLight(
		vec3 position,
		vec3 direction,
		int lightPower = 2,
		float cutOff = glm::cos(glm::radians(12.5f)),
		float outerCutOff = glm::cos(glm::radians(17.5f))
	);

	void MovePosition(vec3 position);
	void MoveDirection(vec3 direction);
	void SyncGUI();

	Light GetLightInfo() {
		return lightInfo;
	}

	int GetCascadeMaxLevel() {
		return cascadeLevel;
	}

	std::vector<glm::mat4> GetLightSpaceMatrices() {
		return Shadow->GetLightSpaceMatrices(lightInfo);
	}

	std::vector<float> GetCasacdeLevel() {
		return Shadow->GetCascadeLevels();
	}
};

