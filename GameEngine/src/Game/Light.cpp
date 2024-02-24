#include "Light.h"
#include "../Util/Shader.h"

// 0번 <- 태양 예약 1~12번까지 사용가능.
std::vector<LightPower> Light::lightPowers = {
	{1.0f,0.0,0.0f}, // 태양 빛 감쇠 없음.
	{1.0f,0.7f,1.8f}, // distance 7
	{1.0f,0.35f,0.44f}, // distance 13.7
	{1.0f,0.22f,0.20f}, // distance 20
	{1.0f,0.14f,0.07f}, // distance 32
	{1.0f,0.09f,0.032f}, // distance 50
	{1.0f,0.07f,0.017f}, // distance 65
	{1.0f,0.045f,0.0075f}, // distance 100
	{1.0f,0.027f,0.0028f}, // distance 160
	{1.0f,0.022f,0.0019f}, // distance 200
	{1.0f,0.014f,0.0007f}, // distance 325
	{1.0f,0.007f,0.0002f}, // distance 600
	{1.0f,0.0014f,0.000007f}, // distance 3250
};

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

	const auto constant = lightPowers[lightPower].constant;
	const auto linear = lightPowers[lightPower].linear;
	const auto quadratic = lightPowers[lightPower].quadratic;

	std::cout << lightPower <<  " " << quadratic << std::endl;

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
