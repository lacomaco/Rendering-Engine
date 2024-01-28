#pragma once
#include "../ECS/ECS.h"
#include "../Component/MoveComponent.h"
#include "../Component/BoxComponent.h"
#include <SDL.h>

class BallMoveSystem : public System{
public:
	BallMoveSystem();
	void Update(double deltaTime, int windowHeight,int windowWidth);
};

