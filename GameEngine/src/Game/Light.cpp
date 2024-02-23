#include "Light.h"
#include "../Util/Shader.h"

Light::Light() {
	box = make_shared<Box>();
	box->SetTexture("./assets/images/white.png", "albedo");
	box->SetupMesh();
	box->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	box->position = glm::vec3(1.0f, 1.0f, 0.0f);
}

void Light::Draw(const char* shaderProgramName) {
	box->Draw(shaderProgramName);
}

void Light::PutLightUniform(const char* shaderProgramName,int lightPosition) {
	const auto& shader = Shader::getInstance();

	shader->setVec3(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].position").c_str(), getPosition());
	shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].constant").c_str(), constant);
	shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].linear").c_str(), linear);
	shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].quadratic").c_str(), quadratic);
	shader->setVec3(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].direction").c_str(), direction);
	shader->setInt(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].lightType").c_str(), lightType);

	if (lightType == 0 || lightType == 2) {
		shader->setVec3(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].direction").c_str(), direction);
	}
	if (lightType == 2) {
		shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].cutOff").c_str(), cutOff);
		shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].cutOuter").c_str(), cutOuter);
	}
}	
