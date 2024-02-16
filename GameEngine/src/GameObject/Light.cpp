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
	shader->setVec3(shaderProgramName, "light.strength", strength);
	shader->setFloat(shaderProgramName, "light.fallOffStart", fallOffStart);
	shader->setFloat(shaderProgramName, "light.fallOffEnd", fallOffEnd);
	shader->setVec3(shaderProgramName, "light.direction", direction);
	shader->setFloat(shaderProgramName, "light.spotPower", spotPower);
	shader->setInt(shaderProgramName, "light.lightType", lightType);
	shader->setFloat(shaderProgramName, "light.specularStrength", specularStrength);
}	
