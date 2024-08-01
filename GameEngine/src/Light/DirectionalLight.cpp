#include "DirectionalLight.h"
#include "../Editor/EditorSharedValue.h"

DirectionalLight::DirectionalLight(
	vec3 position,
	vec3 direction,
	int lightPower,
	float cutOff,
	float outerCutOff) {
	lightInfo.direction = direction;
	lightInfo.isShadowLight = true;

	lightInfo.position = position;
	lightInfo.lightType = 0;

	lightInfo.strength = glm::vec3(2.0);
	auto light = lightPowers[lightPower];
	lightInfo.constant = light.constant;

	lightInfo.linear = light.linear;
	lightInfo.quadratic = light.quadratic;

	lightInfo.cutOff = cutOff;
	lightInfo.cutOuter = outerCutOff;
	
	Shadow = make_shared<CascadeShadow>(cascadeLevel);
}

void DirectionalLight::MovePosition(vec3 position) {
	lightInfo.position = position;
}

void DirectionalLight::MoveDirection(vec3 direction) {
	lightInfo.direction = direction;
}

void DirectionalLight::SyncGUI() {
	MovePosition(EditorSharedValue::directionalLightPosition);
	MoveDirection(EditorSharedValue::directionalLightDirection);
}
