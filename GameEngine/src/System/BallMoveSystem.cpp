#include "BallMoveSystem.h"
#include "../Component/MoveComponent.h"
#include "../Component/BoxComponent.h"
#include "../Util/QuadTree.h"
BallMoveSystem::BallMoveSystem()
{
	RequireComponent<MoveComponent>();
	RequireComponent<BoxComponent>();
}

void BallMoveSystem::Update(double deltaTime,int windowHeight,int windowWidth) {
	QuadTree quad = QuadTree(QuadTreeRectangle(0, 0, windowWidth, windowHeight), -1);
	std::vector<QuadTreeRectangle> returnObjects;

	for (auto& [id,entity] : GetSystemEntities()) {
		if (entity.HasTag("paddle")) {
			continue;
		}
		auto& move = entity.GetComponent<MoveComponent>();
		auto& box = entity.GetComponent<BoxComponent>();
		quad.Insert(QuadTreeRectangle(move.x, move.y, box.width, box.height, entity.GetId()));
	}

	for (auto& [id, entity] : GetSystemEntities()) {
		auto& move = entity.GetComponent<MoveComponent>();
		auto& box = entity.GetComponent<BoxComponent>();

		move.Update(deltaTime);

		returnObjects.clear();
		quad.Retrieve(returnObjects, QuadTreeRectangle(move.x, move.y, box.width, box.height, entity.GetId()));

		for (auto& collisionCheckEntity : returnObjects) {
			if (entity.GetId() == collisionCheckEntity.entityId) {
				continue;
			}

			auto collisionEntity = getEntityById(collisionCheckEntity.entityId);
			if (collisionEntity) {
				auto& collisionMove = collisionEntity->GetComponent<MoveComponent>();
				auto& collisionBox = collisionEntity->GetComponent<BoxComponent>();

				if (move.x < collisionMove.x + collisionBox.width &&
					move.x + box.width > collisionMove.x &&
					move.y < collisionMove.y + collisionBox.height &&
					move.y + box.height > collisionMove.y) {
					// 충돌이 감지됨

					// 겹침 정도 계산
					float overlapX = std::min(move.x + box.width, collisionMove.x + collisionBox.width) - std::max(move.x, collisionMove.x);
					float overlapY = std::min(move.y + box.height, collisionMove.y + collisionBox.height) - std::max(move.y, collisionMove.y);

					// 어느 방향으로 보정할지 결정
					if (overlapX < overlapY) {
						// X 축 방향으로 보정
						if (move.x < collisionMove.x) {
							move.x -= overlapX;
						}
						else {
							move.x += overlapX;
						}
						move.velocity.x *= -1; // 속도 반전
					}
					else {
						// Y 축 방향으로 보정
						if (move.y < collisionMove.y) {
							move.y -= overlapY;
						}
						else {
							move.y += overlapY;
						}
						move.velocity.y *= -1; // 속도 반전
					}
				}
			}
		}

		if (move.y < 0) {
			move.y = 0;
			move.velocity.y *= -1;
		}
		else if (move.y + box.height > windowHeight) {
			move.y = windowHeight - box.height;
			move.velocity.y *= -1;
		}

		if (move.x < 0) {
			move.x = 0;
			move.velocity.x *= -1;
		}
		else if (move.x + box.width > windowWidth) {
			move.x = windowWidth - box.width;
			move.velocity.x *= -1;
		}
	}
}
