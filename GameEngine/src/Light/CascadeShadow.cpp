#include "CascadeShadow.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../Game/CameraShareValue.h"
#include "../Constants.h"
#include <math.h>
#include <iostream>

CascadeShadow::CascadeShadow(float cascadeLevel) {
	for (int i = 1; i <= cascadeLevel; i++) {
		float level = CameraShareValue::far / std::pow(2.0f, cascadeLevel - i);
		cascadeLevels.push_back(level);
	}
}

vector<glm::vec4> CascadeShadow::GetFrustumCornerWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
	const glm::mat4 inverseProjectionView = glm::inverse(proj * view);

	std::vector<glm::vec4> frustumCorners;

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				glm::vec4 point = glm::vec4(x*2.0f - 1.0f, y*2.0f - 1.0f, z*2.0f - 1.0f, 1.0f);
				glm::vec4 worldPoint = inverseProjectionView * point;
				worldPoint = worldPoint / worldPoint.w;

				frustumCorners.push_back(worldPoint);
			}
		}
	}

	return frustumCorners;
}

glm::mat4 CascadeShadow::GetDirectionalLightMatrix(const float near, const float far,const glm::vec3 lightDirection) {
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
		center + lightDirection,
		center,
		glm::vec3(0.0,1.0,0.0)
	);

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();

	for (const auto& v : corners) {
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	// ortho 평면 넓히기용.

	const float zMult = 10.0f;

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

	const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}

std::vector<glm::mat4> CascadeShadow::GetLightSpaceMatrices(glm::vec3 lightDirection) {
	std::vector<glm::mat4> matrices;
	for (int i = 0; i < cascadeLevels.size(); i++) {
		if (i == 0) {
			matrices.push_back(GetDirectionalLightMatrix(CameraShareValue::near, cascadeLevels[0], lightDirection));
		}
		else if (i == cascadeLevels.size() - 1) {
			matrices.push_back(GetDirectionalLightMatrix(cascadeLevels[i], CameraShareValue::far, lightDirection));
		}
		else {
			matrices.push_back(GetDirectionalLightMatrix(cascadeLevels[i],cascadeLevels[i+1], lightDirection));
		}
	}

	return matrices;
}
