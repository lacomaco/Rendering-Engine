#include "BaseObject.h"
#include "../../Util/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;


void BaseObject::PutModelUniform(const char* shaderProgramName) {
	auto shader = Shader::getInstance();

	glm::mat4 model = glm::mat4(1.0f);

	/*
	* 이미 움직인 물체를 피킹해서 회전시킬경우 짐벌락이 발생할 수 있기 때문에 쿼터니언으로 처리.
	*/
	glm::vec3 rotationRadians = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotationRadians);
	glm::mat4 rotationMatrix = glm::toMat4(quaternion);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

	// 연산자 순서 주의. 뒤에서 부터 연산된다.
	// translationMatrix가 rotationMatrix보다 먼저 연산될 경우 회전이동이 된다. 이에 주의할것
	model = translationMatrix * rotationMatrix * scaleMatrix;

	shader->setMat4(shaderProgramName, "model", model);

	glm::mat4 invTranspose = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	invTranspose = glm::inverse(invTranspose);
	invTranspose = glm::transpose(invTranspose);
	shader->setMat4(shaderProgramName, "invTranspose", invTranspose);

}