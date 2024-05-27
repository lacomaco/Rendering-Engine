#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "../Util/Shader.h"
#include "./MainSceneTexture.h"
#include "./IconTexture.h"
#include <memory>

class GridGui
{
private:
	void SceneUpdate();
	void ContentBrowserUpdate();
	void InspectorUpdate();
	void EngineOptionUpdate();

	std::shared_ptr<MainSceneTexture> mainSceneTexture;
	std::shared_ptr<IconTexture> folderIcon;
	std::shared_ptr<IconTexture> fbxIcon;

public:
	std::shared_ptr<IconTexture> xmlIcon;

	unsigned int getMainSceneFrameBuffer() {
		if (!mainSceneTexture) {
			return 0;
		}

		return mainSceneTexture->getMainSceneFrameBuffer();
	}

	GridGui(SDL_Window* window, SDL_GLContext context);
	~GridGui();
	void Update();
	void Render();
};

