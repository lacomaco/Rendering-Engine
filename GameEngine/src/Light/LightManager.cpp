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

	glGenBuffers(1, &shadowUniformBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, shadowUniformBlock);
	glBufferData(GL_UNIFORM_BUFFER, 3904, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, SHADOW_UBO, shadowUniformBlock, 0, 6416);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glGenTextures(1, &shadowMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,
		GL_DEPTH_COMPONENT32F,
		SHADOW_RESOLUTION,
		SHADOW_RESOLUTION,
		61,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		nullptr
	);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMaps, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void LightManager::BindLightUBO(const char* shaderProgramName) {
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

void LightManager::UpdateLightUBO() {
	// Lights 사이즈 생성
	int stack = 0;
	const int maxLights = 100;
	std::vector<Light> lights;
	lights.reserve(maxLights);


	glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBlock);

	// light 배열 업로드.

	// Sun 추가
	if (Sun) {
		lights.push_back(Sun->GetLightInfo());
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
	
	int directionalCascadeLevel = Sun->GetCascadeLevel();
	glBufferSubData(GL_UNIFORM_BUFFER, stack, sizeof(int), &directionalCascadeLevel);
	stack += sizeof(int);


	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::BindShadowUBO(const char* shaderProgramName) {
	const auto shader = Shader::getInstance();
	const auto program = shader->getShaderProgram(shaderProgramName);

	GLuint blockIndex = glGetUniformBlockIndex(program, "LightSpaceMatrices");
	if (blockIndex == GL_INVALID_INDEX) {
		std::cerr << "Error: Unable to find uniform block index for 'Lights'" << std::endl;
	}
	else {
		glUniformBlockBinding(program, blockIndex, SHADOW_UBO);
	}
}

void LightManager::UpdateShadowUBO() {
	std::vector<mat4> lightMatrices;

	glBindBuffer(GL_UNIFORM_BUFFER, shadowUniformBlock);

	const auto sunLightMatrices = Sun->GetLightSpaceMatrices();

	lightMatrices.insert(lightMatrices.end(), sunLightMatrices.begin(), sunLightMatrices.end());

	// Spot Light 처리

	// Point Light 처리

	glBufferSubData(GL_UNIFORM_BUFFER, 0, lightMatrices.size() * sizeof(glm::mat4), lightMatrices.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::CreateShadow(
	shared_ptr<MeshRenderer> meshRenderer
) {
	int index = 0;

	// Point Spot 작업

	int totalLightMatricesCount = 0;

	if (isUseSun) {
		totalLightMatricesCount += Sun->GetCascadeLevel();
	}

	if (isUsePointLight) {
		totalLightMatricesCount += pointLights.size() * 6;
	}

	if (isUseSpotLight) {
		totalLightMatricesCount += spotLights.size();
	}

	auto shader = Shader::getInstance();
	const char* shaderProgramName = "cascade-shadow";
	glUseProgram(shader->getShaderProgram(shaderProgramName));
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, shadowMaps, 0);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	// Mesh Renderer Instancing 모드
	meshRenderer->Draw(shaderProgramName,totalLightMatricesCount);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<DirectionalLight> LightManager::GetSun() {
	return Sun;
}