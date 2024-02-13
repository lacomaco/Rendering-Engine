#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	// 75 권장.
	Camera(float radian, int width, int height);
	float near = 0.1f;
	float far = 100.0f;
	glm::mat4 projection;

	// 여기는 아직 카메라 미구현이라. 상수처리.
	glm::mat4 view;

	void putCameraUniform(const char* shaderProgramName);
};

