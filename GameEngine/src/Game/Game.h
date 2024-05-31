#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>
#include "../Editor/GridGui.h"
#include "../GameObject/PrimitiveObject/Plane.h"
#include "../GameObject/PrimitiveObject/Box.h"
#include "../GameObject/PrimitiveObject/Circle.h"
#include "../Game/Camera.h"
#include "../Game/Input.h"
#include "../GameObject/PrimitiveObject/Model.h"
#include "MeshRenderer.h"
#include <memory>
#include "./GraphicsPipeLine.h"
#include "../GameObject/PrimitiveObject/CubeMap.h"

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
	SDL_Window* window;
	SDL_GLContext context;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void CreateShaderProgram();

	SDL_Window* mWindow;
	bool mIsRunning;
	Uint32 mTicksCount;

	GridGui* gridGui;

	std::shared_ptr<Plane> plane;

	std::vector<std::shared_ptr<Box>> box;
	std::shared_ptr<CubeMap> cubeMap;

	std::vector<std::shared_ptr<Circle>> circle;

	std::vector<std::shared_ptr<Circle>> pbrTestCircle;
	std::vector<float> pbrTestMetallic;
	std::vector<float> pbrTestRoughness;

	std::shared_ptr<Camera> camera;

	Input* input;

	std::shared_ptr<Model> model;

	std::vector<std::shared_ptr<Plane>> grass;

	shared_ptr<MeshRenderer> meshRenderer;

	shared_ptr<Model> backPack;

	shared_ptr<GraphicsPipeLine> graphicsPipe;

	float accTime;

	bool modelOn = true;
};

