#pragma once
#include <glm/glm.hpp>

class Camera
{
	// projection,
	// view,
	Camera(float radian, float width, float height);

	glm::vec3 cameraPosition;
	glm::vec3 lookAt;
	glm::vec3 up;
};

