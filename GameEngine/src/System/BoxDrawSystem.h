#pragma once
#include "../ECS/ECS.h"
#include "../Component/BoxComponent.h"
#include "../Component/MoveComponent.h"
#include <SDL.h>
#include <iostream>
#include "../Util/QuadTree.h"
#include <SDL_ttf.h>

class BoxDrawSystem : public System {
public:
	BoxDrawSystem();

	void Draw(SDL_Renderer* renderer, TTF_Font* font);
	void DrawQuadTree(int windowHeight, int windowWidth, SDL_Renderer* renderer);
};