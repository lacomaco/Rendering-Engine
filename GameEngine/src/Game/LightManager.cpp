#include "LightManager.h"
#include <random>
#include "../Util/Shader.h"

LightManager::LightManager(int _activeLight)
{
	activeLight = _activeLight;
}

void LightManager::PutLightUniform(const char* programName) {
	for(int i = 0; i < activeLight; i++) {
		auto light = lights[i];
		light->PutLightUniform(programName,i);
	}
}

void LightManager::DrawLight(Camera* camera) {
	auto program = Shader::getInstance()->getShaderProgram("light");
	glUseProgram(program);
	camera->putCameraUniform("light");
	for (int i = 0; i < activeLight; i++) {
		auto light = lights[i];
		light->PutLightUniform("light", i);
		light->Draw("light");
	}
}


void LightManager::UpdateLight(float deltaTime) {
	for (int i = 0; i < activeLight; i++) {
		auto light = lights[i];
		if (light->lightType == 1) {
			light->Update(deltaTime);
		}
	}
}

void LightManager::SetRandomLight(Camera* camera) {
	std::mt19937 mt{std::random_device{}()};
	std::uniform_int_distribution<int> dist(0, 2);

	for (int i = 0; i < activeLight; i++) {
		int random = dist(mt);

		std::cout << "light 생성 타입 : " << random << std::endl;
		auto light = make_shared<Light>();
		light->lightType = random;
		light->box->scale = glm::vec3(0.05f, 0.05f, 0.05f);
		light->direction = camera->cameraFront;

		// random 포지션 x : 0 ~ 2, y : 0 ~ 2, z : 0 ~ 2
		int randomX = dist(mt);
		int randomY = dist(mt);
		int randomZ = dist(mt);

		light->setPosition(glm::vec3(randomX, randomY, randomZ));

		lights[i] = light;
	}
}

