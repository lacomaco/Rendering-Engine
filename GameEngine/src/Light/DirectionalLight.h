#pragma once
#include "glm/glm.hpp"
#include "LightType.h"
#include "CascadeShadow.h"

using namespace glm;


class DirectionalLight
{
private:
	Light lightInfo;
	CascadeShadow CascadeShadow;

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
	void CreateShadow();

	Light getLightInfo() {
		return lightInfo;
	}
};

