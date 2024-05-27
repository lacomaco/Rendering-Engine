#include "GridGui.h"
#include <iostream>
#include "../Game/Input.h"
#include "../Editor/EditorSharedValue.h"
#include "RenderingDebugPanel.h"

/*
* Imgui SDL2 with OpenGL ���� ����
* 
* https://github.com/ocornut/imgui/blob/master/examples/example_sdl2_opengl3/main.cpp
*/
GridGui::GridGui(SDL_Window* window, SDL_GLContext context) {
	// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	// ��Ʈ��Ʋ��
	ImFont* font = io.Fonts->AddFontDefault();
	io.Fonts->Build();

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 460"); // ���⼭ ������ GLSL ������ �����ϼ���.

	mainSceneTexture = std::make_shared<MainSceneTexture>();
	folderIcon = std::make_shared<IconTexture>("./icons/folder.png");
	xmlIcon = std::make_shared<IconTexture>("./icons/xml.png");
	fbxIcon = std::make_shared<IconTexture>("./icons/fbx.png");

}

void GridGui::Update() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (EditorSharedValue::editorMode) {
		SceneUpdate();
		ContentBrowserUpdate();
		InspectorUpdate();
	}
}


void GridGui::Render() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GridGui::SceneUpdate() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(960, 720));
	ImGui::Begin("Render Output", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginTabBar("Scene Viewer")) {
		if (ImGui::BeginTabItem("Main Scene")) {
			ImGui::Image(
				(void*)(intptr_t)mainSceneTexture->getMainSceneTexture(),
				ImVec2(960, 720),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Mesh View")) {
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void GridGui::ContentBrowserUpdate() {
	ImGui::SetNextWindowPos(ImVec2(0, 720));
	ImGui::SetNextWindowSize(ImVec2(1280, 240));
	ImGui::Begin("Content Browser", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Image(
		(void*)(intptr_t)folderIcon->getTexture(),
		ImVec2(32, 32)
	);
	ImGui::SameLine();
	ImGui::Text("are you stupid?");


	// ���ؽ�Ʈ �޴� ����
	if (ImGui::BeginPopupContextItem("item context menu"))
	{
		if (ImGui::MenuItem("New Level"))
		{
			// "New Level" �׸��� ���õǾ��� �� ������ �ڵ�
			// ��: ���ο� ������ �����ϴ� �Լ� ȣ��
			std::cout << "New Level Created!" << std::endl;
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

void GridGui::InspectorUpdate() {
	ImGui::SetNextWindowPos(ImVec2(960, 0));
	ImGui::SetNextWindowSize(ImVec2(320, 720));
	ImGui::Begin("Game Panel", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	if(ImGui::BeginTabBar("Inspector Tab Bar")) {
		if (ImGui::BeginTabItem("Inspector")) {
			ImGui::Text("component Inspector");
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Rendering Debug Panels")) {
			RenderingDebugPanel::UpdateRenderingDebugPanel();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();
}



GridGui::~GridGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}