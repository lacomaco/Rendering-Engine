#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	// 45보다 작은값 권장.
	Camera(float fov, int width, int height);
	float near = 0.1f;
	float far = 100.0f;
	glm::mat4 projection;

	// 여기는 아직 카메라 미구현이라. 상수처리.
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;
	glm::vec3 cameraUp;

	glm::mat4 view;

	void putCameraUniform(const char* shaderProgramName);
};

