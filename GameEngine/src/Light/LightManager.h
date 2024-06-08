#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "./LightType.h"
#include "DirectionalLight.h"

using namespace glm;

class LightManager
{
private:
	std::shared_ptr<DirectionalLight> Sun;
	std::vector<Light> spotLights;
	std::vector<Light> pointLights;
	std::vector<Light> virtualLights;

	bool isUseSun = true;
	bool isUseSpotLight = false;
	bool isUsePointLight = false;

	unsigned int lightUniformBlock;
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

	void BindUBO(const char* shaderProgramName);
	void UpdateUBO();

	std::shared_ptr<DirectionalLight> GetSun();
};

