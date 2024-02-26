#pragma once
#include "Light.h"
#include "Camera.h"
#include <memory>
#include "glm/glm.hpp"
#include "MeshRenderer.h"

class LightManager
{
private:
	static LightManager* instance;

	int directionalLightCount = 0; // max: 1
	int pointLightCount = 0; // max: 2
	int spotLightCount = 0; // max : 2

public:
	LightManager() {};

	~LightManager() = default;

	LightManager& operator=(const LightManager&) = delete;

	static LightManager* getInstance() {
		if (LightManager::instance == nullptr) {
			instance = new LightManager();
		}
		return instance;
	}

	void UpdateLight(float deltaTime);
	void SetRandomLight(shared_ptr<Camera> camera);
	vector<shared_ptr<Light>> lights;

	void PutLightUniform(const char* programName);
	void DrawLight(shared_ptr<Camera> camera);
	bool CreateLight(
		int lightType,
		glm::vec3 position,
		glm::vec3 direction,
		int lightPower = 2,
		float cutOff = glm::cos(glm::radians(12.5f)),
		float outerCutOff = glm::cos(glm::radians(17.5f))
	);

	void MakeShadow(shared_ptr<MeshRenderer> meshRenderer);
};

