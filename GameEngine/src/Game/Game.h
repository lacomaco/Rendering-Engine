#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>
#include "../GameObject/Actor.h"
#include "./ImguiController.h"
#include "../GameObject/PrimitiveObject/Plane.h"
#include "../GameObject/PrimitiveObject/Box.h"
#include "../GameObject/PrimitiveObject/Circle.h"
#include "../Game/Camera.h"
#include "../Game/Input.h"
#include "../GameObject/PrimitiveObject/Model.h"
#include "./Light.h"
#include "./LightManager.h"
#include "MeshRenderer.h"
#include <memory>

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
	SDL_Window* window;
	SDL_GLContext context;

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void CreateShaderProgram();

	SDL_Window* mWindow;
	bool mIsRunning;
	Uint32 mTicksCount;

	ImguiController* imguiController;


	std::vector<SDL_Scancode> mLeftPaddleKeys;

	bool mUpdatingActors;

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;

	std::shared_ptr<Plane> plane;

	std::vector<std::shared_ptr<Box>> box;

	std::vector<std::shared_ptr<Circle>> circle;

	std::shared_ptr<Camera> camera;

	Input* input;

	std::shared_ptr<Model> model;

	int activeLight = 3;
	std::shared_ptr<LightManager> lightManager;

	std::vector<std::shared_ptr<Plane>> grass;

	shared_ptr<MeshRenderer> meshRenderer;

	shared_ptr<Model> backPack;

	float accTime;
};

