#include "Camera.h"
#include "../Util/Shader.h"
#include <SDL.h>

Camera::Camera(float fov, int width, int height) {
	projection = glm::perspective(fov, float(width) / height, near, far);
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
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
		cameraTarget,
		cameraUp
	);
}

void Camera::Update() {
	auto time = SDL_GetTicks() / 5000.0f;
	float camX = sin(time * 3.0f) * radius;
	float camZ = cos(time * 3.0f) * radius;

	cameraPos = glm::vec3(camX, 0.0f, camZ);
	cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	view = glm::lookAt(
		cameraPos,
		cameraTarget,
		cameraUp
	);
}

void Camera::putCameraUniform(const char* shaderProgramName) {
	auto shader = Shader::getInstance();

	shader->setMat4(shaderProgramName,"projection", projection);
	shader->setMat4(shaderProgramName,"view", view);
}