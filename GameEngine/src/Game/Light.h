#pragma once
#include "../GameObject/PrimitiveObject/Box.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Shadow.h"
#include "PointShadow.h"
#include "ShadowInterface.h"

class Shadow;

struct LightPower {
	float constant;
	float linear;
	float quadratic;
};


class Light
{
private:
	static std::vector<LightPower> lightPowers;

	// 그림자 계산을 위한 near,far값.
	// 멀리있는 물체는 그림자를 생성하지 않는다.
	float near = 0.1f;
	float far = 30.0f;

public:
	shared_ptr<Box> box;

	shared_ptr<ShadowInterface> shadow;

	Light(int lightType);
	glm::mat4 lightSpaceMatrix;
	void Draw(const char* shaderProgramName);
	// 0 : directional , 1 : point, 2 : spot
	int lightType = 1;

	// 선택함.
	int lightPower = 3;

	bool isDisable = false;

	// directional light, spot light 에 사용.
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

	glm::vec3 strength = glm::vec3(1.0f, 1.0f, 1.0f);

	float cutOff = glm::cos(glm::radians(12.5f));
	float cutOuter = glm::cos(glm::radians(17.5f));

	// 그림자 매핑시 시야 변환을 위한 유니폼 생성용 함수.
	void MakeShadow(shared_ptr<MeshRenderer> meshRenderer);
	void PutShadowMap(const char* shaderProgramName,int index, int current);
	void MakeDirectionalShadow(const char* shaderProgramName, shared_ptr<MeshRenderer> meshRenderer);
	void MakePointShadow(const char* shaderProgramName, shared_ptr<MeshRenderer> meshRenderer);

	void PutLightUniform(const char* shaderProgramName, int lightPosition);

	glm::vec3 getPosition();

	void setPosition(glm::vec3 position);
	void lookHere(glm::vec3 point);

	glm::quat yRotationQuat = glm::angleAxis(
		glm::radians(0.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	void Update(float deltaTime);
	void CalculateLightSpaceMatrix();
};

