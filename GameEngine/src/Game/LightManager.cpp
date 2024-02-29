#include "LightManager.h"
#include <random>
#include "../Util/Shader.h"

LightManager* LightManager::instance = nullptr;


bool LightManager::CreateLight(
	int lightType,
	glm::vec3 position,
	glm::vec3 direction,
	int lightPower,
	float cutOff,
	float outerCutOff
) {
	if (lightType == 0 && directionalLightCount == 1) {
		std::cout << "directionalLightCount 초과" << std::endl;
		return false;
	}

	if (lightType == 1 && pointLightCount == 2) {
		std::cout << "pointLightCount 초과" << std::endl;
		return false;
	}

	if (lightType == 2 && spotLightCount == 2) {
		std::cout << "spotLightCount 초과" << std::endl;
		return false;
	}

	auto light = make_shared<Light>(lightType);
	light->direction = glm::normalize(direction);
	light->setPosition(position);
	light->lightPower = lightPower;
	light->cutOff = cutOff;
	light->cutOuter = outerCutOff;

	if (lightType == 0) {
		directionLights.push_back(light);
		directionalLightCount++;
	}
	else if (lightType == 1) {
		pointLights.push_back(light);
		pointLightCount++;
	}
	else if (lightType == 2) {
		directionLights.push_back(light);
		spotLightCount++;
	}

	return true;
}

void LightManager::PutLightUniform(const char* programName) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(programName);
	glUseProgram(program);

	shader->setInt(programName, "lightCount", directionLights.size());
	shader->setInt(programName, "pointLightCount", pointLights.size());


	// set 0 to all shadows.
	shader->setBool(programName, "directionalShadowMap.use", 0);
	shader->setBool(programName, "pointShadowMap[0].use", 0);
	shader->setBool(programName, "spotShadowMap[0].use", 0);
	shader->setBool(programName, "pointShadowMap[1].use", 0);
	shader->setBool(programName, "spotShadowMap[1].use", 0);

	int directionalCount = 0;
	int spotCount = 0;

	for (int i = 0; i < directionLights.size(); i++) {
		auto& light = directionLights[i];
		light->PutLightUniform(programName, i);

		int index = 0;
		if (light->lightType == 0) {
			index = directionalCount;
			directionalCount++;
		}
		else if (light->lightType == 2) {
			index = spotCount;
			spotCount++;
		}

		light->PutShadowMap(programName, index,i);
	}

	for (int i = 0; i < pointLights.size(); i++) {
		auto& light = pointLights[i];
		light->PutLightUniform(programName, i);

		// 쉐도우 깊이맵 텍스처 바인딩 겹치는걸 피하기 위해서 index를 directionLight 다음으로 설정함.
		light->PutShadowMap(programName, i,i + directionLights.size());
	}
}

void LightManager::DrawLight(shared_ptr<Camera> camera) {
	auto program = Shader::getInstance()->getShaderProgram("light");
	glUseProgram(program);
	camera->putCameraUniform("light");
	for (int i = 0; i < directionLights.size(); i++) {
		auto& light = directionLights[i];
		light->PutLightUniform("light", i);
		light->Draw("light");
	}

	for (int i = 0; i < pointLights.size(); i++) {
		auto& light = pointLights[i];
		light->PutLightUniform("light", i);
		light->Draw("light");
	}
}


void LightManager::UpdateLight(float deltaTime) {
	for (auto& light : directionLights) {
		light->Update(deltaTime);
	}

	for (auto& light : pointLights) {
		light->Update(deltaTime);
	}
}

void LightManager::SetRandomLight(shared_ptr<Camera> camera) {
	std::mt19937 mt{std::random_device{}()};
	std::uniform_int_distribution<int> dist(0, 2);

	for (int i = getTotalLightCount(); i < 5; i++) {
		int random = dist(mt);

		int randomX = dist(mt);
		int randomY = dist(mt);
		int randomZ = dist(mt);
		glm::vec3 position = glm::vec3(randomX, randomY, randomZ);


		while (!CreateLight(random,position,camera->cameraFront)) {
			random = dist(mt);
		}
	}
}

void LightManager::MakeShadow(shared_ptr<MeshRenderer> meshRenderer) {
	for (auto& light : directionLights) {
		light->MakeShadow(meshRenderer);
	}

	for (auto& light : pointLights) {
		light->MakeShadow(meshRenderer);
	}
}
