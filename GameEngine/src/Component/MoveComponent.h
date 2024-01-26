#pragma once
#include <glm/glm.hpp>
#include <iostream>

struct MoveComponent {
	float x;
	float y;
	glm::vec2 velocity;
	MoveComponent(float x=0, float y=0, glm::vec2 velocity = glm::vec2(0.0f,0.0f)) : 
		x(x), 
		y(y),
		velocity(velocity)
	{
	}

	void setVelocity(glm::vec2 velocity) {
		this->velocity = velocity;
	}

	void Update(double deltaTime) {
		x += velocity.x * deltaTime;
		y += velocity.y * deltaTime;
	}
};
