#include "LightManager.h"
#include <random>
#include "../Util/Shader.h"
#include "../Constants.h"
#include "../Util/DummyTexture2D.h"
#include "../Util/DummyTextureCube.h"

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

	int activeDirecitonalLight = 0;

	for (int i = 0; i < directionLights.size(); i++) {
		auto light = directionLights[i];
		if (!light->isDisable) {
			activeDirecitonalLight++;
		}
	}

	int activePointLight = 0;
	for (int i = 0; i < pointLights.size(); i++) {
		auto light = pointLights[i];
		if (!light->isDisable) {
			activePointLight++;
		}
	}

	shader->setInt(programName, "lightCount", activeDirecitonalLight);
	shader->setInt(programName, "pointLightCount", activePointLight);


	// set 0 to all shadows.
	shader->setBool(programName, "directionalShadowMap.use", 0);
	shader->setBool(programName, "pointShadowMap[0].use", 0);
	shader->setBool(programName, "spotShadowMap[0].use", 0);
	shader->setBool(programName, "pointShadowMap[1].use", 0);
	shader->setBool(programName, "spotShadowMap[1].use", 0);

	int textureOffset = TEXTURE_SKYBOX_OFFSET;

	int directionalCount = 0;

	// directional light 처리.
	int putDirectionCount = 0;
	for (int i = 0; i < directionLights.size(); i++) {
		auto& light = directionLights[i];
		if (light->lightType == 0 && !light->isDisable) {
			light->PutLightUniform(programName, putDirectionCount);
			light->PutShadowMap(programName, putDirectionCount, textureOffset);
			textureOffset++;
			putDirectionCount++;
		}
	}

	auto dummyTexture = DummyTexture2D::getInstance();
	// directional light를 안넣었다면 더미 sampler2D를 넣어준다.

	if (putDirectionCount == 0) {
		glActiveTexture(GL_TEXTURE0 + textureOffset);
		shader->setBool(programName, "directionalShadowMap.use", 0);
		glBindTexture(GL_TEXTURE_2D, dummyTexture->dummyTexture2D);
		shader->setInt(programName, "directionalShadowDepthMap", textureOffset);
		textureOffset++;
	}

	// spotLight 처리
	int spotCount = 0;

	for (int i = 0; i < directionLights.size(); i++) {
		auto& light = directionLights[i];
		if (light->lightType == 2 && !light->isDisable) {
			light->PutLightUniform(programName, spotCount);
			light->PutShadowMap(programName, spotCount, textureOffset);
			textureOffset++;
			spotCount++;
		}
	}

	// spotLight 안넣은만큼 더미 sampler2D를 넣어준다.
	for (int i = spotCount; i < 2; i++) {
		glActiveTexture(GL_TEXTURE0 + textureOffset);
		glBindTexture(GL_TEXTURE_2D, dummyTexture->dummyTexture2D);
		shader->setBool(programName, ("spotShadowMap[" + std::to_string(i) + "].use").c_str(), 0);
		shader->setInt(programName, ("spotShadowDepthMap[" + std::to_string(i) + "]").c_str(), textureOffset);
		textureOffset++;
	}


	auto dummyCubeTexture = DummyTextureCube::getInstance();
	int pointCount = 0;

	for (int i = 0; i < pointLights.size(); i++) {
		auto& light = pointLights[i];
		if (light->isDisable) {
			continue;
		}

		light->PutLightUniform(programName, i);
		// 쉐도우 깊이맵 텍스처 바인딩 겹치는걸 피하기 위해서 index를 directionLight 다음으로 설정함.
		light->PutShadowMap(programName, i,textureOffset);
		pointCount++;
		textureOffset++;
	}

	// pointLight 안넣은만큼 더미 samplerCube를넣어준다.
	for (int i = pointCount; i < 2; i++) {
		glActiveTexture(GL_TEXTURE0 + textureOffset);
		glBindTexture(GL_TEXTURE_CUBE_MAP, dummyCubeTexture->dummyTextureCube);
		shader->setInt(programName, ("pointShadowDepthMap[" + std::to_string(i) + "]").c_str(), textureOffset);
		textureOffset++;
	}
}

void LightManager::DrawLight(shared_ptr<Camera> camera) {
	auto program = Shader::getInstance()->getShaderProgram("light");
	glUseProgram(program);
	for (int i = 0; i < directionLights.size(); i++) {
		auto& light = directionLights[i];

		if (light->isDisable) continue;

		light->PutLightUniform("light", i);
		light->Draw("light");
	}

	for (int i = 0; i < pointLights.size(); i++) {
		auto& light = pointLights[i];

		if(light->isDisable) continue;

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
		if(light->isDisable) continue;

		light->MakeShadow(meshRenderer);
	}

	for (auto& light : pointLights) {
		if(light->isDisable) continue;
		light->MakeShadow(meshRenderer);
	}
}

void LightManager::DisablePointLight(int index) {
	pointLights[index]->isDisable = true;
}

void LightManager::DisableDirectionalLight(int index) {
	directionLights[index]->isDisable = true;
}

void LightManager::EnablePointLight(int index) {
	pointLights[index]->isDisable = false;
}

void LightManager::EnableDirectionalLight(int index) {
	directionLights[index]->isDisable = false;
}
