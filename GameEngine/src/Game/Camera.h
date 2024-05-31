#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
	float radius = 3.0f;
	float currentYaw = 0.0f;
	float currentPitch = 0.0f;

	float targetYaw = 0.0f;
	float targetPitch = 0.0f;

	float xSensitive = 1.0;
	float ySensitive = 0.5;

	void CalculateCameraDirection();
	
	unsigned int cameraUniformBlock;

public:
	Camera(float fov, int width, int height);
	glm::mat4 projection;

	// 여기는 아직 카메라 미구현이라. 상수처리.
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;
	glm::vec3 cameraUp;

	void Update(float deltaTime);
	void CameraLookAround(float deltaTime);

	void ResetPitch();

	float cameraMove = 2.5f;

	void ImguiUpdate();
	void bindUBO(const char* shaderProgramName);
	void UpdateUBO();

};

