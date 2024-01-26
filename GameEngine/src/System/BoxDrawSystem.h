#pragma once
#include "../ECS/ECS.h"
#include "../Component/BoxComponent.h"
#include "../Component/MoveComponent.h"
#include <SDL.h>
#include <iostream>

class BoxDrawSystem : public System {
public:
	BoxDrawSystem() {
		RequireComponent<BoxComponent>();
		RequireComponent<MoveComponent>();
	}

	void Update(SDL_Renderer* renderer) {
		for (auto entity : GetSystemEntities()) {
			auto& boxComponent = entity.GetComponent<BoxComponent>();
			auto& moveComponent = entity.GetComponent<MoveComponent>();
			SDL_Rect rect = {
				moveComponent.x,
				moveComponent.y,
				boxComponent.width,
				boxComponent.height
			};
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

			SDL_RenderFillRect(
				renderer,
				&rect
			);
		}
	}
};