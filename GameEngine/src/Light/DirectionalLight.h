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

	void GetLightSpaceMatrices(
		std::vector<mat4>& lightMatrices,
		std::vector<glm::mat4>& invProjMatrices,
		std::vector<float>& radius
	) {
		Shadow->GetLightSpaceMatrices(
			lightInfo,
			lightMatrices,
			invProjMatrices,
			radius
		);
	}

	std::vector<float> GetCasacdeLevel() {
		return Shadow->GetCascadeLevels();
	}
};

