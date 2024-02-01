#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
	SDL_Renderer* mRenderer;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	bool mIsRunning;
	Uint32 mTicksCount;

	std::vector<SDL_Scancode> mLeftPaddleKeys;

};

