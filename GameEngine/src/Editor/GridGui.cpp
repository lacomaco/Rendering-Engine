#include "GridGui.h"
#include <iostream>
#include "../Game/Input.h"

GridGui* GridGui::instance = nullptr;
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

	if (editorMode) {
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

void GridGui::PutPBRUniform(const char* programName) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(programName);
	glUseProgram(program);
	shader->setFloat(programName, "metallicTest", metallic);
	shader->setFloat(programName, "roughnessTest", roughness);

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

		if (ImGui::BeginTabItem("Engine Options")) {
			EngineOptionUpdate();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();
}

void GridGui::EngineOptionUpdate() {
	ImGuiIO& io = ImGui::GetIO();

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

	ImGui::SliderFloat("cameraSpeed", &cameraSpeed, 0.0f, 10.0f);

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
			ImVec2(200, 200),
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

	if (ImGui::TreeNode("show SSAO Texture")) {
		ImGui::Text("SSAO Texture");
		ImGui::Image(
			(void*)(intptr_t)ssaoTexture,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::Text("SSAO Blur Texture");
		ImGui::Image(
			(void*)(intptr_t)ssaoBlurTexture,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::SliderFloat("radius", &radius, 0.0f, 5.0f);
		ImGui::SliderFloat("bias", &bias, -1.0f, 1.0f);
		ImGui::Checkbox("useSSAO", &useSSAO);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Lens Flare")) {
		ImGui::SliderInt("ghosts", &uGhosts, 0, 10);
		ImGui::SliderFloat("ghostDispersal", &uGhostDispersal, 0.0f, 1.0f);
		ImGui::Image(
			(void*)(intptr_t)lensFlareTexture,
			ImVec2(200, 200),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ImGui::TreePop();
	}
}



GridGui::~GridGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}