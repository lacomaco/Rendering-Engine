#include "Light.h"
#include "../Util/Shader.h"
#include <string>
#include <sstream>
#include "../Constants.h"


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
	{1.0f,0.0014f,0.000007f}, // distance 3250 // 12
};

Light::Light() {
	box = make_shared<Box>();
	box->SetTexture("./assets/images/white.png", "albedo");
	box->SetupMesh();
	box->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	box->position = glm::vec3(1.0f, 1.0f, 0.0f);

	shadow = make_shared<Shadow>();
}

void Light::Draw(const char* shaderProgramName) {
	box->Draw(shaderProgramName);
}

void Light::CalculateLightSpaceMatrix() {
	glm::mat4 lightProjection, lightView;

	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightRight = glm::normalize(glm::cross(up, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);

	lightView = glm::lookAt(
		getPosition(),
		direction,
		lightUp
	);

	lightSpaceMatrix = lightProjection * lightView;
}

void Light::MakeShadow(const char* shaderProgramName, shared_ptr<MeshRenderer> meshRenderer) {
	CalculateLightSpaceMatrix();

	shadow->WriteDepthMap(meshRenderer, lightSpaceMatrix);
}


void Light::PutLightUniform(const char* shaderProgramName,int lightPosition) {
	const auto& shader = Shader::getInstance();

	const auto constant = lightPowers[lightPower].constant;
	const auto linear = lightPowers[lightPower].linear;
	const auto quadratic = lightPowers[lightPower].quadratic;

	shader->setVec3(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].position").c_str(), getPosition());
	shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].constant").c_str(), constant);
	shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].linear").c_str(), linear);
	shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].quadratic").c_str(), quadratic);
	shader->setVec3(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].direction").c_str(), direction);
	shader->setInt(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].lightType").c_str(), lightType);
	shader->setVec3(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].strength").c_str(), strength);

	if (lightType == 0 || lightType == 2) {
		shader->setVec3(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].direction").c_str(), direction);
	}
	if (lightType == 2) {
		shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].cutOff").c_str(), cutOff);
		shader->setFloat(shaderProgramName, ("lights[" + std::to_string(lightPosition) + "].cutOuter").c_str(), cutOuter);
	}
}

void Light::Update(float deltaTime) {
	yRotationQuat =
		glm::angleAxis(glm::radians(90.0f) * deltaTime, glm::vec3(0, 1, 0));

	glm::mat4 rotationMatrix = glm::toMat4(yRotationQuat);

	glm::vec3 rotatedPosition = glm::vec3(
		rotationMatrix * glm::vec4(box->position, 1.0f)
	);

	setPosition(rotatedPosition);
	CalculateLightSpaceMatrix();

}

glm::vec3 Light::getPosition() {
	return box->position;
}

void Light::setPosition(glm::vec3 position) {
	box->position = position;
}

void Light::PutShadowMap(const char* shaderProgramName, int index, int current) {
	auto shader = Shader::getInstance();

	if (lightType == 0) {
		shader->setBool(shaderProgramName, "directionalShadowMap.use", 1);
		shader->setMat4(shaderProgramName, "directionalShadowMap.lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_OFFSET + current);
		glBindTexture(GL_TEXTURE_2D, shadow->depthMap);
		shader->setInt(shaderProgramName, "directionalShadowMap.depthMap", TEXTURE_SKYBOX_OFFSET + current);
	}
	else if (lightType == 1) {
		shader->setBool(shaderProgramName, ("pointShadowMap[" + std::to_string(index) + "].use").c_str(), 1);
		shader->setMat4(shaderProgramName, ("pointShadowMap[" + std::to_string(index) + "].lightSpaceMatrix").c_str(), lightSpaceMatrix);

		glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_OFFSET + current);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow->depthMap);
		shader->setInt(shaderProgramName, ("pointShadowMap[" + std::to_string(index) + "].depthMap").c_str(), TEXTURE_SKYBOX_OFFSET + current);
	}
	else if (lightType == 2) {
		shader->setBool(shaderProgramName, ("spotShadowMap[" + std::to_string(index) + "].use").c_str(), 1);
		shader->setMat4(shaderProgramName, ("spotShadowMap[" + std::to_string(index) + "].lightSpaceMatrix").c_str(), lightSpaceMatrix);

		glActiveTexture(GL_TEXTURE0 + TEXTURE_SKYBOX_OFFSET + current);
		glBindTexture(GL_TEXTURE_2D, shadow->depthMap);
		shader->setInt(shaderProgramName, ("spotShadowMap[" + std::to_string(index) + "].depthMap").c_str(), TEXTURE_SKYBOX_OFFSET + current);
	}
}
