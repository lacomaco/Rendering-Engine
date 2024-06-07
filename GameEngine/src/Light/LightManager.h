#pragma once
#include <glm/glm.hpp>

using namespace glm;

class LightManager
{
private:

public:
	LightManager();
	void ToggleDirectionalLight();
	void TogglePointLight();
	void ToggleSpotLight();
	void CreateLight(
		int lightType,
		vec3 position,
		vec3 direction,
		int lightPower = 2,
		float cutOff = glm::cos(glm::radians(12.5f)),
		float outerCutOff = glm::cos(glm::radians(17.5f))
	);

	void GetSun();
};

