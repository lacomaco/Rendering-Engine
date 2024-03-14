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
	ImGui_ImplOpenGL3_Init("#version 460"); // 여기서 적절한 GLSL 버전을 설정하세요.

}

void ImguiController::Update() {
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// 임구이 세팅
	ImGui::Begin("Game Engine Controller");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::Text(
		"Camera Position (x:%.3f y:%.3f z:%.3f)",
		cameraPosition.x,
		cameraPosition.y,
		cameraPosition.z
	);

	ImGui::Text(
		"Camera Front (x:%.3f y:%.3f z:%.3f)",
		cameraFront.x,
		cameraFront.y,
		cameraFront.z
	);

	ImGui::Text("Exposure");
	ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.0f);

	ImGui::Checkbox("useTexture", &useTexture);
	ImGui::Checkbox("showNormal", &showNormal);

	if (ImGui::TreeNode("DirectionalLight")) {
		ImGui::Text("Position");
		ImGui::SliderFloat("x", &directionalLightPosition.x, -20.0f, 20.0f);
		ImGui::BeginDisabled();
		ImGui::SliderFloat("y", &directionalLightPosition.y, -20.0f, 20.0f);
		ImGui::EndDisabled();
		ImGui::SliderFloat("z", &directionalLightPosition.z, -20.0f, 20.0f);

		ImGui::Text("Direction");
		ImGui::SliderFloat("x", &directionalLightDirection.x, -1.0f, 1.0f);
		ImGui::BeginDisabled();
		ImGui::SliderFloat("y", &directionalLightDirection.y, -1.0f, 1.0f);
		ImGui::EndDisabled();
		ImGui::SliderFloat("z", &directionalLightDirection.z, -1.0f, 1.0f);

		ImGui::Text("shadow depthMap");
		ImGui::Image(
			(void*)(intptr_t)directionalLightDepthMap,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("CubeMap Test")) {
		if (ImGui::RadioButton("skybox", select == 0)) {
			select = 0;
		}

		if (ImGui::RadioButton("radiance", select == 1)) {
			select = 1;
		}

		if (ImGui::RadioButton("irradiance", select == 2)) {
			select = 2;
		}

		ImGui::SliderFloat("lodLevel", &lodLevel, 0.0f, 6.0f);

		ImGui::TreePop();
	}

	// TODO: 나중에 지우기.
	if (ImGui::TreeNode("PBR Test")) {
		ImGui::Text("Metallic");
		ImGui::SliderFloat("metallic", &metallic, 0.0f, 1.0f);

		ImGui::Text("Roughness");
		ImGui::SliderFloat("roughness", &roughness, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Bloom")) {
		ImGui::Text("bloomThreshold");
		ImGui::SliderFloat("bloomCount", &bloomThreshold, 0, 1.0);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("light")) {
		ImGui::Checkbox("useSun", &useSun);
		ImGui::Checkbox("usePointLight", &usePointLight);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("for god ray debugging")) {
		ImGui::Image(
			(void*)(intptr_t)godLightTexture,
			ImVec2(200,200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::SliderFloat("decay", &decay, 0.0f, 1.0f);
		ImGui::SliderFloat("density", &density, 0.0f, 1.0f);
		ImGui::SliderFloat("weight", &weight, 0.0f, 1.0f);
		ImGui::SliderFloat("exposure", &godRayExposure, 0.0f, 1.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("show GBuffer")) {
		ImGui::Text("Position Metallic Texture");
		ImGui::Image(
			(void*)(intptr_t)positionMetallicTexture,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::Text("Albedo Roughness Texture");
		ImGui::Image(
			(void*)(intptr_t)albedoRoughnessTexture,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::Text("Normal Texture");
		ImGui::Image(
			(void*)(intptr_t)normalTexture,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::Text("God Ray Texture");
		ImGui::Image(
			(void*)(intptr_t)godRayTexture,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::TreePop();
	}


	ImGui::End();

}


void ImguiController::Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// TODO: 나중에 지우기.
void ImguiController::PutPBRUniform(const char* programName) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(programName);
	glUseProgram(program);
	shader->setFloat(programName, "metallicTest", metallic);
	shader->setFloat(programName, "roughnessTest", roughness);

}



ImguiController::~ImguiController() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}