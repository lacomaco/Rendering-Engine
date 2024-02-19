#include "LightManager.h"
#include <random>
#include "../Util/Shader.h"

LightManager::LightManager(int _activeLight)
{
	activeLight = _activeLight;
}

void LightManager::CreateLight(
	int lightType,
	glm::vec3 position,
	glm::vec3 direction,
	float constant,
	float linear,
	float quadratic,
	float cutOff,
	float outerCutOff
) {
	if (lights.size() == activeLight) {
		std::cout << "라이트 갯수 초과" << std::endl;
		return;
	}

	auto light = make_shared<Light>();
	light->lightType = lightType;
	light->direction = direction;
	light->setPosition(position);
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
	light->cutOff = cutOff;
	light->cutOuter = outerCutOff;

	lights.push_back(light);
}

void LightManager::PutLightUniform(const char* programName) {
	for(int i = 0; i < lights.size(); i++) {
		auto light = lights[i];
		light->PutLightUniform(programName,i);
	}
}

void LightManager::DrawLight(Camera* camera) {
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

void LightManager::SetRandomLight(Camera* camera) {
	std::mt19937 mt{std::random_device{}()};
	std::uniform_int_distribution<int> dist(0, 2);

	for (int i = lights.size(); i < activeLight; i++) {
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

		lights.push_back(light);
	}
}

