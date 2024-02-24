#pragma once
#include "Light.h"
#include "Camera.h"
#include <memory>
#include "glm/glm.hpp"

class LightManager
{
private:
	int maxLights;

public:
	LightManager(int _maxLights);
	~LightManager() = default;

	void UpdateLight(float deltaTime);
	void SetRandomLight(shared_ptr<Camera> camera, int lightType = -1);
	vector<shared_ptr<Light>> lights;

	void PutLightUniform(const char* programName);
	void DrawLight(shared_ptr<Camera> camera);
	void CreateLight(
		int lightType,
		glm::vec3 position,
		glm::vec3 direction,
		int lightPower = 2,
		float cutOff = glm::cos(glm::radians(12.5f)),
		float outerCutOff = glm::cos(glm::radians(17.5f))
	);
};

