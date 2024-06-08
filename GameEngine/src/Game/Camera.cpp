#include "Camera.h"
#include "../Util/Shader.h"
#include <SDL.h>
#include "Input.h"
#include "../Util/CommonMath.h"
#include <math.h>
#include <glm/gtc/quaternion.hpp>
#include "../Editor/EditorSharedValue.h"
#include "CameraShareValue.h"
#include "../Constants.h"
#include <glm/gtc/type_ptr.hpp>


Camera::Camera(float fov, int width, int height) {
	glGenBuffers(1, &cameraUniformBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBlock);
	glBufferData(GL_UNIFORM_BUFFER, 212, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, CAMERA_UBO, cameraUniformBlock, 0, 212);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error Occur when Camera UBO created " << error << std::endl;
	}


	projection = glm::perspective(glm::radians(fov), float(width) / height, CameraShareValue::near, CameraShareValue::far);
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

	CameraShareValue::view = glm::lookAt(
		cameraPos,
		cameraDirection,
		cameraUp
	);

	EditorSharedValue::cameraSpeed = cameraMove;
}

void Camera::bindUBO(const char* shaderProgramName) {
	const auto shader = Shader::getInstance();
	const auto program = shader->getShaderProgram(shaderProgramName);


	GLuint blockIndex = glGetUniformBlockIndex(program, "Camera");
	if (blockIndex == GL_INVALID_INDEX) {
		std::cerr << "Error: Unable to find uniform block index for 'Camera'" << std::endl;
	}
	else {
		glUniformBlockBinding(program, blockIndex, CAMERA_UBO);
	}
}

void Camera::UpdateUBO() {
	int stack = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBlock);
	glBufferSubData(
		GL_UNIFORM_BUFFER,
		stack,
		sizeof(glm::mat4),
		glm::value_ptr(projection)
	);
	stack += sizeof(glm::mat4);

	glBufferSubData(
		GL_UNIFORM_BUFFER,
		stack,
		sizeof(glm::mat4),
		glm::value_ptr(CameraShareValue::view)
	);
	stack += sizeof(glm::mat4);

	auto skyboxView = glm::lookAt(
		glm::vec3(0.0f),
		cameraFront,
		cameraUp
	);

	glBufferSubData(
		GL_UNIFORM_BUFFER,
		stack,
		sizeof(glm::mat4),
		glm::value_ptr(skyboxView)
	);
	stack += sizeof(glm::mat4);

	glBufferSubData(
		GL_UNIFORM_BUFFER,
		stack,
		sizeof(glm::vec3),
		glm::value_ptr(cameraPos)
	);
	stack += sizeof(glm::vec3);

	float padding = 0.0f;
	glBufferSubData(
		GL_UNIFORM_BUFFER,
		stack,
		sizeof(float),
		&padding
	);
	stack += sizeof(float);


	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
	ImguiUpdate();
	const auto& Input = Input::GetInstance();
	const float cameraSpeed = cameraMove * deltaTime;


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

	if(Input->IsKeyPressed(SDL_SCANCODE_SPACE)) {
		cameraPos += cameraSpeed * cameraUp;
	}

	if (Input->IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		cameraPos -= cameraSpeed * cameraUp;
	}

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);

	CameraShareValue::view = glm::lookAt(
		cameraPos,
		cameraPos + cameraFront,
		cameraUp
	);

	EditorSharedValue::cameraPosition = cameraPos;
	EditorSharedValue::cameraFront = cameraFront;

	UpdateUBO();
}

void Camera::ImguiUpdate() {
	cameraMove = EditorSharedValue::cameraSpeed;
}


