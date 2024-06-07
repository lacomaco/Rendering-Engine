#include "Input.h"
#include "../Constants.h"
#include <iostream>

Input* Input::instance = nullptr;

Input::Input() {
	state = SDL_GetKeyboardState(NULL);
	SetMouse();
}

glm::vec2 Input::ScreenNormalize(int x, int y) {
	glm::vec2 point;
	point.x = (2.0f * x) / WINDOW_WIDTH - 1.0f;
	point.y = 1.0f - (2.0f * y) / WINDOW_HEIGHT;
	return point;
}

void Input::SetMouse() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	mousePos = ScreenNormalize(x, y);
}

