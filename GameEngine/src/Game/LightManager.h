#pragma once
#include "Light.h"
#include "Camera.h"
#include <memory>

class LightManager
{
private:
	int activeLight;
	shared_ptr<Light> lights[8];

public:
	LightManager(int _activeLight);
	~LightManager() = default;

	void UpdateLight(float deltaTime);
	void SetRandomLight(Camera* camera);

	void PutLightUniform(const char* programName);
	void DrawLight(Camera* camera);
};

