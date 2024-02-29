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
		std::cout << "directionalLightCount 段引" << std::endl;
		return false;
	}

	if (lightType == 1 && pointLightCount == 2) {
		std::cout << "pointLightCount 段引" << std::endl;
		return false;
	}

	if (lightType == 2 && spotLightCount == 2) {
		std::cout << "spotLightCount 段引" << std::endl;
		return false;
	}

	auto light = make_shared<Light>(lightType);
	light->direction = glm::normalize(direction);
	light->setPosition(position);
	light->lightPower = lightPower;
	light->cutOff = cutOff;
	light->cutOuter = outerCutOff;

	lights.push_back(light);

	return true;
}

void LightManager::PutLightUniform(const char* programName) {
	auto shader = Shader::getInstance();
	shader->setInt(programName, "lightCount", lights.size());

	for(int i = 0; i < lights.size(); i++) {
		auto light = lights[i];
		light->PutLightUniform(programName,i);
	}


	// set 0 to all shadows.
	shader->setBool(programName, "directionalShadowMap.use", 0);
	shader->setBool(programName, "pointShadowMap[0].use", 0);
	shader->setBool(programName, "spotShadowMap[0].use", 0);
	shader->setBool(programName, "pointShadowMap[1].use", 0);
	shader->setBool(programName, "spotShadowMap[1].use", 0);

	int directionalCount = 0;
	int pointCount = 0;
	int spotCount = 0;

	for (int i = 0; i < lights.size(); i++) {
		auto light = lights[i];

		int index = 0;

		if (light->lightType == 0) {
			index = directionalCount;
			directionalCount++;
		}
		else if (light->lightType == 1) {
			index = pointCount;
			pointCount++;
		}
		else if (light->lightType == 2) {
			index = spotCount;
			spotCount++;
		}

		light->PutShadowMap(programName, index,i);
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
		light->Update(deltaTime);
	}
}

void LightManager::SetRandomLight(shared_ptr<Camera> camera) {
	std::mt19937 mt{std::random_device{}()};
	std::uniform_int_distribution<int> dist(0, 2);

	for (int i = lights.size(); i < 5; i++) {
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
	for (int i = 0; i < lights.size(); i++) {
		auto light = lights[i];
		light->MakeShadow(meshRenderer);
	}
}
