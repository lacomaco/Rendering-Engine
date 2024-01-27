#include "PaddleMoveSystem.h"
#include "../Component/BoxComponent.h"
#include <SDL.h>


PaddleMoveSystem::PaddleMoveSystem()
{
	RequireComponent<KeyboardControlledComponent>();
	RequireComponent<MoveComponent>();
}

void PaddleMoveSystem::Update(double deltaTime, int windowHeight) {
	for (auto entity : GetSystemEntities()) {
		if (!entity.HasTag("paddle")) {
			continue;
		}

		auto& keyInput = entity.GetComponent<KeyboardControlledComponent>();
		auto& move = entity.GetComponent<MoveComponent>();
		auto& box = entity.GetComponent<BoxComponent>();

		move.velocity.y = 0.0f;
		move.velocity.x = 0.0;

		if (Input::GetKey(KeyCode::W)) {
			move.velocity.y = -500.0f;
		}
		else if (Input::GetKey(KeyCode::S)) {
			move.velocity.y = 500.0f;
		}

		move.Update(deltaTime);

		// 화면 넘어가는지 체크
		if (move.y < 0) {
			move.y = 0;
		} else if (move.y + box.height > windowHeight) {
			move.y = windowHeight - box.height;
		}
	}
}