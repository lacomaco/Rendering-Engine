#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "../Util/Shader.h"
class ImguiController
{

public:
	static ImguiController* instance;
	ImguiController(SDL_Window* window, SDL_GLContext context);
	~ImguiController();
	void Update();
	void Render();

	float f = 0.0f;
	glm::vec3 clear_color = glm::vec3(0.45f, 0.55f, 0.60f);
	int counter = 0;

	ImguiController& operator=(const ImguiController&) = delete; // 할당 연산자 비활성화
	ImguiController(const ImguiController&) = delete; // 복사 생성자 비활성화

	static void CreateInstance(SDL_Window* window, SDL_GLContext context) {
		instance = new ImguiController(window, context);
	}

	static ImguiController* getInstance() {
		return instance;
	}

	// imgui 인풋값들.
	bool useTexture = false;

	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	bool showNormal = false;

	// exposure
	float exposure = 1.0f;

	// directional light
	glm::vec3 directionalLightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 directionalLightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	unsigned int directionalLightDepthMap = 0;

	// Bloom 관련
	float bloomThreshold = 0.04;

	// cube맵 관련
	int select = 0; // 0: skybox, 1: radiance, 2: irradiance
	float lodLevel = 0.0f;

	// PBR 테스트용.
	// TODO: 나중에 지우기.
	float metallic = 0.78f;
	float roughness = 0.0f;

	// 라이트
	bool useSun = true;
	bool usePointLight = false;

	// god light
	unsigned int godLightTexture;
	float decay = 0.0;
	float density = 0.0;
	float weight = 0.0;
	float godRayExposure = 0.0;

	// GBUFFER 텍스처 시각화.
	unsigned int positionMetallicTexture;
	unsigned int albedoRoughnessTexture;
	unsigned int normalTexture;
	unsigned int godRayTexture;

	// SSAO
	unsigned int ssaoTexture;
	unsigned int ssaoBlurTexture;

	float radius = 0.5f;
	float bias = 0.025f;
	bool useSSAO = true;

	void PutPBRUniform(const char* programName);
};

