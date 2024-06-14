#include "CascadeShadow.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../Game/CameraShareValue.h"
#include "../Constants.h"
#include <math.h>
#include <iostream>
#include "../Util/Shader.h"

CascadeShadow::CascadeShadow(float cascadeLevel) {
	for (int i = 1; i <= cascadeLevel; i++) {
		float level = CameraShareValue::far / std::pow(2.0f, cascadeLevel - i);
		cascadeLevels.push_back(level);
	}
}

vector<glm::vec3> CascadeShadow::GetFrustumCornerWorldSpace(const glm::mat4& matrix) {
	const glm::mat4 inverseProjectionView = glm::inverse(matrix);

	std::vector<glm::vec3> frustumCorners;


	std::vector<glm::vec4> ndcCorners = {
		glm::vec4(-1, -1, -1, 1),
		glm::vec4(1, -1, -1, 1),
		glm::vec4(-1, 1, -1, 1),
		glm::vec4(1, 1, -1, 1),
		glm::vec4(-1, -1, 1, 1),
		glm::vec4(1, -1, 1, 1),
		glm::vec4(-1, 1, 1, 1),
		glm::vec4(1, 1, 1, 1)
	};

	for (const auto& corner : ndcCorners) {
		glm::vec4 worldCorner = inverseProjectionView * corner;
		frustumCorners.push_back(glm::vec3(worldCorner) / worldCorner.w);
	}



	return frustumCorners;
}


vector<glm::vec3> CascadeShadow::GetFrustumCornerWorldSpace(
	const glm::mat4& proj, const glm::mat4& view) {
	return GetFrustumCornerWorldSpace(proj * view);
}


glm::mat4 CascadeShadow::GetDirectionalLightMatrix(
	const float near,
	const float far,
	const Light light) {
	const glm::mat4 projection = glm::perspective(
		glm::radians(CameraShareValue::fov),
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
		near,
		far
	);

	const auto corners = GetFrustumCornerWorldSpace(projection, CameraShareValue::view);
	glm::vec3 center = glm::vec3(0);

	for (const auto& corner : corners) {
		center += glm::vec3(corner);
	}

	center /= corners.size();

	const auto lightView = glm::lookAt(
		center - light.direction,
		center,
		glm::vec3(0.0, 1.0, 0.0)
	);

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();

	for (const auto& v : corners) {
		const auto trf = lightView * vec4(v,1.0f);
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	// 각 값을 구분하기 쉽게 출력
	std::cout << "minX: " << minX << std::endl;
	std::cout << "maxX: " << maxX << std::endl;
	std::cout << "minY: " << minY << std::endl;
	std::cout << "maxY: " << maxY << std::endl;
	std::cout << "minZ: " << minZ << std::endl;
	std::cout << "maxZ: " << maxZ << std::endl;

	// ortho 평면 넓히기용.

	const float zMult = 5.0f;

	if (minZ < 0) {
		minZ *= zMult;
	}
	else {
		minZ /= zMult;
	}

	if (maxZ < 0) {
		maxZ /= zMult;
	}
	else {
		maxZ *= zMult;
	}

	glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}

std::vector<glm::mat4> CascadeShadow::GetLightSpaceMatrices(Light light) {
	std::vector<glm::mat4> matrices;
	for (int i = 0; i <= cascadeLevels.size(); i++) {
		if (i == 0) {
			std::cout << "구간 " << CameraShareValue::near << " - " << cascadeLevels[0] << std::endl;
			matrices.push_back(GetDirectionalLightMatrix(CameraShareValue::near, cascadeLevels[0], light));
		}
		else if (i == cascadeLevels.size()) {
			matrices.push_back(GetDirectionalLightMatrix(cascadeLevels[i-1], CameraShareValue::far, light));
		}
		else {
			std::cout << "구간 " << cascadeLevels[i - 1] << " - " << cascadeLevels[i] << std::endl;
			matrices.push_back(GetDirectionalLightMatrix(cascadeLevels[i-1],cascadeLevels[i], light));
		}
	}

	return matrices;
}

void CascadeShadow::drawCascadeVolumeVisualizers(Light light) {
	auto shader = Shader::getInstance();
	glUseProgram(shader->getShaderProgram("cascade-debug"));

	std::vector<glm::mat4> lightMatrices = GetLightSpaceMatrices(light);

	visualizerVAOs.resize(8);
	visualizerEBOs.resize(8);
	visualizerVBOs.resize(8);


	const GLuint indices[] = {
		0, 2, 3,
		0, 3, 1,
		4, 6, 2,
		4, 2, 0,
		5, 7, 6,
		5, 6, 4,
		1, 3, 7,
		1, 7, 5,
		6, 7, 3,
		6, 3, 2,
		1, 5, 4,
		0, 1, 4
	};

	const glm::vec4 colors[] = {
		{1.0, 0.0, 0.0, 0.5f},
		{0.0, 1.0, 0.0, 0.5f},
		{0.0, 0.0, 1.0, 0.5f},
	};

	for (int i = 0; i < lightMatrices.size(); ++i)
	{
		const auto corners = GetFrustumCornerWorldSpace(lightMatrices[i]);
		std::vector<glm::vec3> vec3s;
		for (const auto& v : corners)
		{
			vec3s.push_back(glm::vec3(v));
		}

		glGenVertexArrays(1, &visualizerVAOs[i]);
		glGenBuffers(1, &visualizerVBOs[i]);
		glGenBuffers(1, &visualizerEBOs[i]);

		glBindVertexArray(visualizerVAOs[i]);

		glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(visualizerVAOs[i]);
		shader->setVec4("cascade-debug", "color", colors[i % 3]);
		glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

		glDeleteBuffers(1, &visualizerVBOs[i]);
		glDeleteBuffers(1, &visualizerEBOs[i]);
		glDeleteVertexArrays(1, &visualizerVAOs[i]);

		glBindVertexArray(0);
	}

	visualizerVAOs.clear();
	visualizerEBOs.clear();
	visualizerVBOs.clear();
}
