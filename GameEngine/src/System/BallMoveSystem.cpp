#include "BallMoveSystem.h"
#include "../Component/MoveComponent.h"
#include "../Component/BoxComponent.h"
BallMoveSystem::BallMoveSystem()
{
	RequireComponent<MoveComponent>();
	RequireComponent<BoxComponent>();
}

void BallMoveSystem::Update(double deltaTime, int windowHeight) {

}

void BallMoveSystem::CollisionCheck() {

}