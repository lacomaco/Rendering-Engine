#pragma once
#include <SDL.h>
class Game
{
private:
	void ProcessInput();
	void Update();
	void Render();
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	bool isRunning;

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
