#pragma once
#include "../ECS/ECS.h"
class MovementSystem: public System {
public:
	MovementSystem() {
		// TODO: RequireComponent<TransformComponent>();
		// RequireComponent<...>();
	};

	void Update() override {
		for (auto entity : GetEntities()) {
			auto transform = entity->GetComponent<TransformComponent>();
			auto movement = entity->GetComponent<MovementComponent>();

			transform->position += movement->velocity * Time::deltaTime;
		}
	}
};
