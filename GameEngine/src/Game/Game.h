#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>
#include "../GameObject/Actor.h"

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

	SDL_Window* mWindow;
	bool mIsRunning;
	Uint32 mTicksCount;

	std::vector<SDL_Scancode> mLeftPaddleKeys;

	bool mUpdatingActors;

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;

	std::vector<float> vertices = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	std::vector <unsigned int> indices = {
		1, 2, 3,  // second triangle
		0, 1, 3 // first Triangle
	};

	// openGL ฐüทร
	void SetOpenGL();
	unsigned int VBO; // Vertex Buffer Object
	unsigned int VAO; // Vertex Array Object
	unsigned int EBO; // Element Buffer Object


};

