#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "../Util/Shader.h"
#include "./MainSceneTexture.h"

class GridGui
{
private:
	void SceneUpdate();
	void ContentBrowserUpdate();
	void InspectorUpdate();

	MainSceneTexture* mainSceneTexture;

public:
	unsigned int getMainSceneFrameBuffer() {
		if (!mainSceneTexture) {
			return 0;
		}

		return mainSceneTexture->getMainSceneFrameBuffer();
	}

	static GridGui* instance;
	GridGui(SDL_Window* window, SDL_GLContext context);
	~GridGui();
	void Update();
	void Render();

	bool editorMode = true;

	float f = 0.0f;
	glm::vec3 clear_color = glm::vec3(0.45f, 0.55f, 0.60f);
	int counter = 0;

	GridGui& operator=(const GridGui&) = delete; // �Ҵ� ������ ��Ȱ��ȭ
	GridGui(const GridGui&) = delete; // ���� ������ ��Ȱ��ȭ

	static void CreateInstance(SDL_Window* window, SDL_GLContext context) {
		instance = new GridGui(window, context);
	}

	static GridGui* getInstance() {
		return instance;
	}

	// imgui ��ǲ����.
	bool useTexture = false;

	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	bool showNormal = false;
	float cameraSpeed = 2.5f;

	// exposure
	float exposure = 1.0f;

	// directional light
	glm::vec3 directionalLightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 directionalLightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	unsigned int directionalLightDepthMap = 0;

	// Bloom ����
	float bloomThreshold = 0.04;

	// cube�� ����
	int select = 0; // 0: skybox, 1: radiance, 2: irradiance
	float lodLevel = 0.0f;

	// PBR �׽�Ʈ��.
	// TODO: ���߿� �����.
	float metallic = 0.78f;
	float roughness = 0.0f;

	// ����Ʈ
	bool useSun = true;
	bool usePointLight = false;

	// god light
	unsigned int godLightTexture;
	float decay = 0.0;
	float density = 0.0;
	float weight = 0.0;
	float godRayExposure = 0.0;

	// GBUFFER �ؽ�ó �ð�ȭ.
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

	// Lens Flare
	unsigned int lensFlareTexture;
	int uGhosts = 3;
	float uGhostDispersal = 0.4f;

	void PutPBRUniform(const char* programName);
};

