#include "LightManager.h"
#include <random>
#include "../Util/Shader.h"

LightManager* LightManager::instance = nullptr;


void LightManager::CreateLight(
	int lightType,
	glm::vec3 position,
	glm::vec3 direction,
	int lightPower,
	float cutOff,
	float outerCutOff
) {
	if (lights.size() == maxLights) {
		std::cout << "라이트 갯수 초과" << std::endl;
		return;
	}

	auto light = make_shared<Light>();
	light->lightType = lightType;
	light->direction = direction;
	light->setPosition(position);
	light->lightPower = lightPower;
	light->cutOff = cutOff;
	light->cutOuter = outerCutOff;

	lights.push_back(light);
}

void LightManager::PutLightUniform(const char* programName) {
	auto shader = Shader::getInstance();
	shader->setInt(programName, "lightCount", lights.size());

	for(int i = 0; i < lights.size(); i++) {
		auto light = lights[i];
		light->PutLightUniform(programName,i);
	}
}

void LightManager::DrawLight(shared_ptr<Camera> camera) {
	auto program = Shader::getInstance()->getShaderProgram("light");
	glUseProgram(program);
	camera->putCameraUniform("light");
	for (int i = 0; i < lights.size(); i++) {
		auto light = lights[i];
		light->PutLightUniform("light", i);
		light->Draw("light");
	}
}


void LightManager::UpdateLight(float deltaTime) {
	for (int i = 0; i < lights.size(); i++) {
		auto light = lights[i];
		if (light->lightType == 1) {
			light->Update(deltaTime);
		}
	}
}

void LightManager::SetRandomLight(shared_ptr<Camera> camera,int lightType) {
	std::mt19937 mt{std::random_device{}()};
	std::uniform_int_distribution<int> dist(0, 2);

	for (int i = lights.size(); i < maxLights; i++) {
		int random = dist(mt);

		std::cout << "light 생성 타입 : " << random << std::endl;
		auto light = make_shared<Light>();

		if (lightType != -1) {
			light->lightType = lightType;
		}
		else {
			light->lightType = random;
		}
			// random 포지션 x : 0 ~ 2, y : 0 ~ 2, z : 0 ~ 2
		light->box->scale = glm::vec3(0.05f, 0.05f, 0.05f);
		light->direction = camera->cameraFront;

		// random 포지션 x : 0 ~ 2, y : 0 ~ 2, z : 0 ~ 2
		int randomX = dist(mt);
		int randomY = dist(mt);
		int randomZ = dist(mt);

		light->setPosition(glm::vec3(randomX, randomY, randomZ));

		lights.push_back(light);
	}
}

