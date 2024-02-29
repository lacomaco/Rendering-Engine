#include "Camera.h"
#include "../Util/Shader.h"
#include <SDL.h>
#include "Input.h"
#include "../Util/CommonMath.h"
#include <math.h>
#include <glm/gtc/quaternion.hpp>
#include "../Game/ImguiController.h"

Camera::Camera(float fov, int width, int height) {
	projection = glm::perspective(glm::radians(fov), float(width) / height, near, far);
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	/*
	* 주의.
	* 카메라 -> 타겟 벡터가 아닌
	* 카메라 <- 타겟 벡터이다.
	* 
	* 3D 에선 카메라가 움직이는게 아니라 세상이 움직이는 것이기 때문이다.
	* 착각하기 쉬움으로 주석을 남겨놓겠다.
	*/
	cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	view = glm::lookAt(
		cameraPos,
		cameraDirection,
		cameraUp
	);
}

void Camera::CalculateCameraDirection() {
	glm::quat pitchQuat = glm::angleAxis(currentPitch, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat yawQuat = glm::angleAxis(-currentYaw, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::quat finalQuat = yawQuat * pitchQuat;

	cameraDirection = finalQuat * glm::vec3(0.0f, 0.0f, -1.0f); // 기본 전방 벡터에 회전 적용

	cameraFront = glm::normalize(cameraDirection);
}

void Camera::ResetPitch() {
	currentYaw = 0.0f;
	currentPitch = 0.0f;

	targetPitch = 0.0f;
	targetYaw = 0.0f;

	CalculateCameraDirection();
}

void Camera::CameraLookAround(float deltaTime) {
	auto rightMouseButtonDown = Input::GetInstance()->rightMouseButtonDown;

	int x, y;
	SDL_GetMouseState(&x, &y);
	glm::vec2 point = Input::GetInstance()->ScreenNormalize(x, y);
	glm::vec2 prev = Input::GetInstance()->mousePos;

	auto diffX = point.x - prev.x;
	auto diffY = point.y - prev.y;

	if (rightMouseButtonDown) {
		targetYaw += diffX * xSensitive;
		targetPitch += diffY * ySensitive;
	}

	float lerpFactor = 30.0f * deltaTime;

	bool valueChange = false;

	if (std::abs(currentYaw - targetYaw) > 0.0001f ) {
		currentYaw = CommonMath::Lerp(currentYaw, targetYaw, lerpFactor);
		valueChange = true;
	}

	if (std::abs(targetYaw - currentYaw) < 0.0001) {
		currentYaw = targetYaw;
		valueChange = true;
	}

	if (std::abs(currentPitch - targetPitch) > 0.0001f) {
		currentPitch = CommonMath::Lerp(currentPitch, targetPitch, lerpFactor);
		valueChange = true;
	}

	if (std::abs(targetPitch - currentPitch) < 0.0001) {
		currentPitch = targetPitch;
		valueChange = true;
	}

	if (valueChange) {
		CalculateCameraDirection();
	}
}

void Camera::Update(float deltaTime) {
	const auto& Input = Input::GetInstance();
	const float cameraSpeed = 2.5f * deltaTime;


	CameraLookAround(deltaTime);

	if (Input->IsKeyPressed(SDL_SCANCODE_W)) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (Input->IsKeyPressed(SDL_SCANCODE_S)) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (Input->IsKeyPressed(SDL_SCANCODE_A)) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (Input->IsKeyPressed(SDL_SCANCODE_D)) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	view = glm::lookAt(
		cameraPos,
		cameraPos + cameraFront,
		cameraUp
	);


	auto imgui = ImguiController::getInstance();

	imgui->cameraPosition = cameraPos;
	imgui->cameraFront = cameraFront;
}

void Camera::putCameraUniform(const char* shaderProgramName) {
	auto shader = Shader::getInstance();

	shader->setMat4(shaderProgramName,"projection", projection);
	shader->setMat4(shaderProgramName,"view", view);
	shader->setVec3(shaderProgramName,"cameraPos", cameraPos);
}

void Camera::putCameraUniformForSkybox(const char* shaderProgramName) {
	auto shader = Shader::getInstance();

	auto skyboxView = glm::lookAt(
		glm::vec3(0.0f),
		cameraFront,
		cameraUp
	);

	shader->setMat4(shaderProgramName, "projection", projection);
	shader->setMat4(shaderProgramName, "view", skyboxView);
	shader->setVec3(shaderProgramName, "cameraPos", cameraPos);
}