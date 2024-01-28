#include "BoxDrawSystem.h"
#include <SDL_ttf.h>

BoxDrawSystem::BoxDrawSystem()
{
	RequireComponent<BoxComponent>();
	RequireComponent<MoveComponent>();
}

void BoxDrawSystem::Draw(SDL_Renderer* renderer,TTF_Font* font) {
	SDL_Color textColor = { 255, 0, 0, 255 }; // »¡°£»ö

	for (auto& [id, entity] : GetSystemEntities()) {
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

void BoxDrawSystem::DrawQuadTree(int windowHeight, int windowWidth, SDL_Renderer* renderer) {
	QuadTree quad = QuadTree(QuadTreeRectangle(0, 0, windowWidth, windowHeight), -1);
	for (auto& [id, entity] : GetSystemEntities()) {
		if (entity.HasTag("paddle")) {
			continue;
		}
		auto& move = entity.GetComponent<MoveComponent>();
		auto& box = entity.GetComponent<BoxComponent>();
		quad.Insert(QuadTreeRectangle(move.x, move.y, box.width, box.height, entity.GetId()));
	}
	std::vector<QuadTreeRectangle> returnObjects;
	quad.DrawBounds(renderer);
}