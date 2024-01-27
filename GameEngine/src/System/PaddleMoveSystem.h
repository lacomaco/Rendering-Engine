#include "../ECS/ECS.h"
#include "../Component/KeyboardControlledComponent.h"
#include "../Component/MoveComponent.h"
#include "../Input/Input.h"
#include "../Logger/Logger.h"

class PaddleMoveSystem : public System {
public:
	PaddleMoveSystem();
	void Update(double deltaTime, int windowHeight);
};