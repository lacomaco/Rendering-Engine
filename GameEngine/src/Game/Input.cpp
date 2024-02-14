#include "Input.h"
Input* Input::instance = nullptr;

Input::Input() {
	state = SDL_GetKeyboardState(NULL);
}
