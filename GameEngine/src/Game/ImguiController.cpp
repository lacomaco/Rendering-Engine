#include "ImguiController.h"
#include <iostream>

ImguiController* ImguiController::instance = nullptr;
/*
* Imgui SDL2 with OpenGL 참고 예제
* 
* https://github.com/ocornut/imgui/blob/master/examples/example_sdl2_opengl3/main.cpp
*/
ImguiController::ImguiController(SDL_Window* window, SDL_GLContext context) {
	// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	// 폰트아틀라스
	ImFont* font = io.Fonts->AddFontDefault();
	io.Fonts->Build();

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 330"); // 여기서 적절한 GLSL 버전을 설정하세요.

}

void ImguiController::Update() {
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// 임구이 세팅
	ImGui::Begin("Game Engine Controller");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::Checkbox("useTexture", &useTexture);
	// slider -2.0f ~ 2.0f
	ImGui::SliderFloat3("modelTranslation", &modelTranslation.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("modelRotation", &modelRotation.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("modelScale", &modelScale.x, 0.1f, 2.0f);
	ImGui::SliderFloat("viewRoot", &viewRoot, -3.14f, 3.14f);
	ImGui::SliderFloat("materialShininess", &materialShininess, 0.0f, 256.0f);
	if (ImGui::RadioButton("Directional Light", selectLightType == 0)) {
			selectLightType = 0;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Point Light", selectLightType == 1)) {
		selectLightType = 1;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spot Light", selectLightType == 2)) {
		selectLightType = 2;
	}
	ImGui::SliderFloat("materialDiffuse", &materialDiffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("materialSpecular", &materialSpecular, 0.0f, 1.0f);
	ImGui::SliderFloat3("lightPosition", &lightPosition.x, -2.0f, 2.0f);
	ImGui::SliderFloat("lightFallOffStart", &lightFallOffStart, 0.0f, 5.0f);
	ImGui::SliderFloat("lightFallOffEnd", &lightFallOffEnd, 0.0f, 10.0f);
	ImGui::SliderFloat("lightSpotPower", &lightSpotPower, 0.0f, 512.0f);

	ImGui::End();

}


void ImguiController::Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



ImguiController::~ImguiController() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}