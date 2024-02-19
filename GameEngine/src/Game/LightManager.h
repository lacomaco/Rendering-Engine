#pragma once
#include "Light.h"
#include "Camera.h"
#include <memory>
#include "glm/glm.hpp"

class LightManager
{
private:
	int activeLight;

public:
	LightManager(int _activeLight);
	~LightManager() = default;

	void UpdateLight(float deltaTime);
	void SetRandomLight(Camera* camera);
	vector<shared_ptr<Light>> lights;

	void PutLightUniform(const char* programName);
	void DrawLight(Camera* camera);
	void CreateLight(
		int lightType, 
		glm::vec3 position,
		glm::vec3 direction,
		float constant = 1.0f, 
		float linear = 0.09f, 
		float quadratic = 0.032f, 
		float cutOff = glm::cos(glm::radians(12.5f)),
		float outerCutOff = glm::cos(glm::radians(17.5f))
	);
};

