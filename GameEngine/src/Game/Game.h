#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>

struct Paddle {
	glm::vec2 mPaddlePos;
	int mPaddleDir;

	Paddle(float x,float y) {
		mPaddlePos = glm::vec2(x,y);
		mPaddleDir = 0;
	}
};

struct Ball {
	glm::vec2 mBallPos;
	glm::vec2 mBallVel;

	Ball(float x, float y) {
		int random_numberX = rand() % 100 + 100;

		int random_numberY = rand() % 100 + 100;

		int xflip = rand() % 5;
		int yflip = rand() % 5;

		xflip = xflip < 2 ? -1 : 1;
		yflip = yflip < 2 ? -1 : 1;

		mBallPos = glm::vec2(x, y);
		mBallVel = glm::vec2(random_numberX * xflip, random_numberY * yflip);
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
	Uint32 mTicksCount;

	std::vector<Paddle> paddles;
	std::vector<SDL_Scancode> mLeftPaddleKeys;
	std::vector<Ball> balls;

	int paddleH;
	int thickness;

};

