#pragma once
#include "../ECS/ECS.h"

class BallMoveSystem : public System{
public:
	BallMoveSystem();
	void Update(double deltaTime, int windowHeight);
	void CollisionCheck();
};

