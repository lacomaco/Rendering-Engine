#include "BaseObject.h"
#include "../../Util/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;

void BaseObject::PutModelUniform(const char* shaderProgramName) {
	auto shader = Shader::getInstance();
	glUseProgram(shader->getShaderProgram(shaderProgramName));

	glm::mat4 model = glm::mat4(1.0f);

	/*
	* �̹� ������ ��ü�� ��ŷ�ؼ� ȸ����ų��� �������� �߻��� �� �ֱ� ������ ���ʹϾ����� ó��.
	*/
	glm::vec3 rotationRadians = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotationRadians);
	glm::mat4 rotationMatrix = glm::toMat4(quaternion);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

	// ������ ���� ����. �ڿ��� ���� ����ȴ�.
	// translationMatrix�� rotationMatrix���� ���� ����� ��� ȸ���̵��� �ȴ�. �̿� �����Ұ�
	model = translationMatrix * rotationMatrix * scaleMatrix;

	shader->setMat4(shaderProgramName, "model", model);

	glm::mat3 invTranspose = glm::mat3(glm::inverse(model));
	invTranspose = glm::transpose(invTranspose);
	shader->setMat3(shaderProgramName, "invTranspose", invTranspose);
}