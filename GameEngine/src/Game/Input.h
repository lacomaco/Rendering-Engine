#pragma once
#include <SDL.h>

class Input
{
private:
	static Input* instance;
	Input();
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
public:
	static Input* GetInstance() {
		if (instance == nullptr) {
			instance = new Input();
		}

		return instance;
	}

	const Uint8* state;

	bool IsKeyPressed(int key) {
		return state[key];
	}
};

