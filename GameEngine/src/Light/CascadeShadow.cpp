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

	auto temp = -minZ;
	minZ = -maxZ;
	maxZ = temp;

	auto mid = (maxZ - minZ) / 2.0f;
	minZ -= mid * 5.0f;
	maxZ += mid * 5.0f;

	glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}

std::vector<glm::mat4> CascadeShadow::GetLightSpaceMatrices(Light light) {
	std::vector<glm::mat4> matrices;
	for (int i = 0; i <= cascadeLevels.size(); i++) {
		if (i == 0) {
			matrices.push_back(GetDirectionalLightMatrix(CameraShareValue::near, cascadeLevels[0], light));
		}
		else if (i == cascadeLevels.size()) {
			matrices.push_back(GetDirectionalLightMatrix(cascadeLevels[i-1], CameraShareValue::far, light));
		}
		else {
			matrices.push_back(GetDirectionalLightMatrix(cascadeLevels[i-1],cascadeLevels[i], light));
		}
	}

	return matrices;
}
