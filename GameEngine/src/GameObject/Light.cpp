#include "Light.h"
#include "../Util/Shader.h"

Light::Light() {
	box = make_shared<Box>();
	box->SetTexture("./assets/white.png", "albedo");
	box->SetupMesh();
	box->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	box->position = glm::vec3(1.0f, 1.0f, 0.0f);
}

void Light::Draw(const char* shaderProgramName) {
	box->Draw(shaderProgramName);
}

void Light::PutLightUniform(const char* shaderProgramName) {
	const auto& shader = Shader::getInstance();
	shader->setVec3(shaderProgramName, "light.position", getPosition());
	shader->setFloat(shaderProgramName, "light.constant", constant);
	shader->setFloat(shaderProgramName, "light.linear", linear);
	shader->setFloat(shaderProgramName, "light.quadratic", quadratic);
	shader->setVec3(shaderProgramName, "light.direction", direction);
	shader->setInt(shaderProgramName, "light.lightType", lightType);

	shader->setVec3(shaderProgramName, "light.ambient", ambient);
	shader->setVec3(shaderProgramName, "light.diffuse", diffuse);
	shader->setVec3(shaderProgramName, "light.specular", specular);
	if (lightType == 0) {
		shader->setVec3(shaderProgramName, "light.direction", direction);
	}
	else if (lightType == 2) {
		shader->setVec3(shaderProgramName, "light.direction", direction);
		shader->setFloat(shaderProgramName, "light.cutOff", cutOff);
		shader->setFloat(shaderProgramName, "light.cutOuter", cutOuter);
	}
}	
