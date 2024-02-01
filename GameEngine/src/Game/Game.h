#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>

struct Paddle {
	glm::vec2 mPaddlePos;
	int mPaddleDir;

	Paddle(float x,float y) {
		mPaddlePos = glm::vec2(x,y);
		mPaddleDir = 0;
	}
};

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
	glm::vec2 mBallPos;
	glm::vec2 mBallVel;
	Uint32 mTicksCount;

	std::vector<Paddle> paddles;
	std::vector<SDL_Scancode> mLeftPaddleKeys;

	int paddleH;
	int thickness;

};

