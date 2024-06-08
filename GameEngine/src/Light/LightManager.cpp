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

}

std::shared_ptr<DirectionalLight> LightManager::GetSun() {
	return Sun;
}