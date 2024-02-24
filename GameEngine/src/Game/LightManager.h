#pragma once
#include "Light.h"
#include "Camera.h"
#include <memory>
#include "glm/glm.hpp"

class LightManager
{
private:
	int maxLights;
	static LightManager* instance;


public:
	LightManager(int _maxLights) :maxLights(_maxLights) {};

	~LightManager() = default;

	LightManager& operator=(const LightManager&) = delete;

	static LightManager* getInstance(int _maxLights = 0) {
		if (LightManager::instance == nullptr) {
			instance = new LightManager(_maxLights);
		}
		return instance;
	}

	int getMaxLights() const {
		return maxLights;
	}

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

