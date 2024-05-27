#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "./MainSceneTexture.h"
#include "./ContentsBrowser.h"
#include <memory>

class GridGui
{
private:
	void SceneUpdate();
	void InspectorUpdate();

	std::shared_ptr<MainSceneTexture> mainSceneTexture;
	std::shared_ptr<ContentsBrowser> contentsBrowser;

public:
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

