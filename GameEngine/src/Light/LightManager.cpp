#include "LightManager.h"
#include "glew.h"
#include "../Constants.h"
#include "../Util/Shader.h"

LightManager::LightManager() {
	Sun = std::make_shared<DirectionalLight>(
		glm::vec3(0,20,-0.001),
		glm::vec3(0,-1,0.005),
		12
	);

	glGenBuffers(1, &lightUniformBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBlock);
	glBufferData(GL_UNIFORM_BUFFER, 6416, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, LIGHT_UBO, lightUniformBlock, 0, 6416);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::ToggleDirectionalLight() {

}

void LightManager::TogglePointLight() {

}

void LightManager::ToggleSpotLight() {

}

void LightManager::CreateLight(
	int lightType,
	vec3 position,
	vec3 direction,
	int lightPower,
	float cutOff,
	float outerCutOff
) {

}

void LightManager::BindUBO(const char* shaderProgramName) {
	const auto shader = Shader::getInstance();
	const auto program = shader->getShaderProgram(shaderProgramName);

	GLuint blockIndex = glGetUniformBlockIndex(program, "Lights");
	if (blockIndex == GL_INVALID_INDEX) {
		std::cerr << "Error: Unable to find uniform block index for 'Lights'" << std::endl;
	}
	else {
		glUniformBlockBinding(program, blockIndex, LIGHT_UBO);
	}
}

void LightManager::UpdateUBO() {
	// Lights 사이즈 생성
	int stack = 0;
	const int maxLights = 100;
	std::vector<Light> lights;
	lights.reserve(maxLights);


	glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBlock);

	// light 배열 업로드.

	// Sun 추가
	if (Sun) {
		lights.push_back(Sun->getLightInfo());
	}

	for (const auto& light : spotLights) {
		if (lights.size() < maxLights) {
			lights.push_back(light);
		}
	}

	for (const auto& light : pointLights) {
		if (lights.size() < maxLights) {
			lights.push_back(light);
		}
	}

	for (const auto& light : virtualLights) {
		if (lights.size() < maxLights) {
			lights.push_back(light);
		}
	}

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light) * lights.size(), lights.data());

	stack += sizeof(Light) * maxLights;

	int lightCount = Sun ? 1 : 0;

	lightCount += spotLights.size();
	lightCount += pointLights.size();
	lightCount += virtualLights.size();

	glBufferSubData(GL_UNIFORM_BUFFER, stack, sizeof(int), &lightCount);
	stack += sizeof(int);
	
	int directionalCascadeLevel = 5;
	glBufferSubData(GL_UNIFORM_BUFFER, stack, sizeof(int), &directionalCascadeLevel);
	stack += sizeof(int);


	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::shared_ptr<DirectionalLight> LightManager::GetSun() {
	return Sun;
}