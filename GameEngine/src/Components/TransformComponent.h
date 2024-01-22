#pragma once
#include <glm/glm.hpp>

struct TransformComponent {
	glm::vec2 position;
	glm::vec2 scale;
	double rotation;

	TransformComponent() : position(0.0f), scale(1.0f), rotation(0.0f) {}
	TransformComponent(glm::vec2 position, glm::vec2 scale, double rotation) : position(position), scale(scale), rotation(rotation) {}
};
