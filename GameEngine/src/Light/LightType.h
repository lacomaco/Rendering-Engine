#pragma once
#include <glm/glm.hpp>
#include <vector>
using namespace glm;

struct Light {
	vec3 direction;
	bool isShadowLight;
	
	vec3 position;
	int lightType;

	vec3 strength;
	float constant;

	float linear;
	float quadratic;
	float cutOff;
	float cutOuter;
};

struct Shadow {
	mat4 lightMatrices[6];

	int lightIndex;
	int matrixCount;
	float padding2[2];
};

struct LightPower {
	float constant;
	float linear;
	float quadratic;
};

extern std::vector<LightPower> lightPowers;
