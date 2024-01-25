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
	CTRL,
	SPACE,
	SHIFT,
	F1,
};

class Input
{
private:
	static Input* input;

	static std::vector<KeyCode> pressedKeys;

public:
	static void ProcessInput() {
		SDL_Event sdlEvent;
		while (SDL_PollEvent(&sdlEvent)) {
			// ImGui SDL input
			ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
			ImGuiIO& io = ImGui::GetIO();
			int mouseX, mouseY;
			const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
			io.MousePos = ImVec2(mouseX, mouseY);
			io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
			io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
			// Handle core SDL events (close window, key pressed, etc.)
			switch (sdlEvent.type) {
			case SDL_QUIT:
				pressedKeys.push_back(KeyCode::ESCAPE);
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					pressedKeys.push_back(KeyCode::ESCAPE);
				}

				if (sdlEvent.key.keysym.sym == SDLK_F1) {
					pressedKeys.push_back(KeyCode::F1);
				}

				if (sdlEvent.key.keysym.sym == SDLK_w) {
					pressedKeys.push_back(KeyCode::W);
				}

				if (sdlEvent.key.keysym.sym == SDLK_a) {
					pressedKeys.push_back(KeyCode::A);
				}

				if (sdlEvent.key.keysym.sym == SDLK_s) {
					pressedKeys.push_back(KeyCode::S);
				}

				if (sdlEvent.key.keysym.sym == SDLK_d) {
					pressedKeys.push_back(KeyCode::D);
				}

				if (sdlEvent.key.keysym.sym == SDLK_LCTRL) {
					pressedKeys.push_back(KeyCode::CTRL);
				}

				if (sdlEvent.key.keysym.sym == SDLK_RCTRL) {
					pressedKeys.push_back(KeyCode::CTRL);
				}

				if (sdlEvent.key.keysym.sym == SDLK_LSHIFT) {
					pressedKeys.push_back(KeyCode::SHIFT);
				}

				if (sdlEvent.key.keysym.sym == SDLK_RSHIFT) {
					pressedKeys.push_back(KeyCode::SHIFT);
				}

				if (sdlEvent.key.keysym.sym == SDLK_SPACE) {
					pressedKeys.push_back(KeyCode::SPACE);
				}
				break;
			}
		}
	}

	static void ClearPressedKeys() {
		pressedKeys.clear();
	}

	static bool GetKey(KeyCode code) {
		return std::find(
			pressedKeys.begin(),
			pressedKeys.end(),
			code
		) != pressedKeys.end();
	}

	Input() {};

	static Input* GetInstance() {
		if (input == nullptr) {
			input = new Input();
			pressedKeys = std::vector<KeyCode>();
		}

		return input;
	}
};

