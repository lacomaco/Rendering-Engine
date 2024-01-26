#include "PaddleMoveSystem.h"

PaddleMoveSystem::PaddleMoveSystem()
{
	RequireComponent<KeyboardControlledComponent>();
	RequireComponent<MoveComponent>();
}

void PaddleMoveSystem::Update(double deltaTime) {
	for (auto entity : GetSystemEntities()) {
		if (!entity.HasTag("paddle")) {
			continue;
		}

		auto& keyInput = entity.GetComponent<KeyboardControlledComponent>();
		auto& move = entity.GetComponent<MoveComponent>();

		move.velocity.y = 0.0f;
		move.velocity.x = 0.0;

		if (Input::GetKey(KeyCode::W)) {
			move.velocity.y = -500.0f;
		}
		else if (Input::GetKey(KeyCode::S)) {
			move.velocity.y = 500.0f;
		}

		move.Update(deltaTime);
	}
}