#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
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
	glm::vec3 modelTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelScale = glm::vec3(1.0f, 1.0f, 1.0f);
	float viewRoot = 0.0f;
	float materialShininess;
	int selectLightType = 0; // 0 : Directional, 1: Point, 2: Spot
	float materialDiffuse = 0.5f;
	float materialSpecular = 0.5f;
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, -2.0f);
	float lightFallOffStart = 0.0f;
	float lightFallOffEnd = 0.0f;
	float lightSpotPower = 0.0f;

	bool showNormal = false;


};

