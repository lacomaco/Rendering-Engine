#pragma once

#include <vector>
#include <SDL.h>
#include <imgui/imgui_impl_sdl.h>
#include <algorithm>

enum KeyCode {
	ESCAPE,
	W,
	A,
	S,
	D,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	CTRL,
	SPACE,
	SHIFT,
	F1,
	Vertical, // 아직 미구현 w s up down
	Horizontal, // 아직 미구현 a d left right
};

class Input
{
private:
	static Input* input;
	static const Uint8* keyState;

public:
	static void ProcessInput() {
		SDL_Event sdlEvent;
		keyState = SDL_GetKeyboardState(NULL);

		while (SDL_PollEvent(&sdlEvent)) {
			ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
			ImGuiIO& io = ImGui::GetIO();
			int mouseX, mouseY;
			const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
			io.MousePos = ImVec2(mouseX, mouseY);
			io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
			io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		}
	}

	static bool GetKey(KeyCode code) {
		switch (code) {
		case KeyCode::W:
			return keyState[SDL_SCANCODE_W];
		case KeyCode::A:
			return keyState[SDL_SCANCODE_A];
		case KeyCode::S:
			return keyState[SDL_SCANCODE_S];
		case KeyCode::D:
			return keyState[SDL_SCANCODE_D];
		case KeyCode::CTRL:
			return keyState[SDL_SCANCODE_LCTRL];
		case KeyCode::SPACE:
			return keyState[SDL_SCANCODE_SPACE];
		case KeyCode::SHIFT:
			return keyState[SDL_SCANCODE_LSHIFT];
		case KeyCode::F1:
			return keyState[SDL_SCANCODE_F1];
		case KeyCode::ESCAPE:
			return keyState[SDL_SCANCODE_ESCAPE];
		default:
			return false;
		}
	}

	Input() {};

	static Input* GetInstance() {
		if (input == nullptr) {
			input = new Input();
		}

		return input;
	}
};

