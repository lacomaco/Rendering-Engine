#include "Camera.h"
#include "../Util/Shader.h"

Camera::Camera(float radian, int width, int height) {
	projection = glm::perspective(radian, float(width) / height, near, far);

	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
}

void Camera::putCameraUniform(const char* shaderProgramName) {
	auto shader = Shader::getInstance();

	shader->setMat4(shaderProgramName,"projection", projection);
	shader->setMat4(shaderProgramName,"view", view);
}