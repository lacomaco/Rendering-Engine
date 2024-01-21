#pragma once
#include <SDL.h>
#include <glm/glm.hpp>

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
	glm::vec2 playerPosition;
	glm::vec2 playerVelocity;
	int millisecsPreviousFrame = 0;

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
