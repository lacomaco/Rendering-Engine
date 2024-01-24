#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include "../ECS/ECS.h"
#include <memory>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
private:
	void ProcessInput();
	void Update();
	void Render();
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	bool isRunning;
	int millisecsPreviousFrame = 0;

	std::unique_ptr<Registry> registry;

public:
	Game();
	~Game();
	void Initialize();
	void Run();
	void Destroy();
	void Setup();

	int windowWidth;
	int windowHeight;
};
