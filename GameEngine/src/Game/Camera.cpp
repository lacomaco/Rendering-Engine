#include "Camera.h"

Camera::Camera(float radian, float width, float height) {
	cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

}