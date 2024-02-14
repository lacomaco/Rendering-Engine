#include "Input.h"
#include "../Constants.h"
#include <iostream>

Input* Input::instance = nullptr;

Input::Input() {
	state = SDL_GetKeyboardState(NULL);
}

glm::vec2 Input::ScreenNormalize(Sint32 x, Sint32 y) {
	glm::vec2 point;
	point.x = (2.0f * x) / WINDOW_WIDTH - 1.0f;
	point.y = 1.0f - (2.0f * y) / WINDOW_HEIGHT;
	return point;
}


void Input::CameraLookAround(Sint32 x, Sint32 y,Camera* camera) {
	if ((state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) && rightMouseButtonDown) {
		auto point = ScreenNormalize(x, y);
		auto diffX = point.x - mousePos.x;
		auto diffY = point.y - mousePos.y;
		camera->CameraLookAround(diffX, diffY);
		SetMouse(x,y);
	}
}

void Input::SetMouse(Sint32 x, Sint32 y) {
	mousePos = ScreenNormalize(x, y);
}


