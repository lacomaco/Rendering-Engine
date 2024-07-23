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

	float radius = glm::length(corners[0] - corners[6]) / 2.0f;
	radius *= 1.3f;
	float texelPerUnit = (float)SHADOW_RESOLUTION / (radius * 2.0f);

	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(texelPerUnit));

	glm::vec3 zeroVector = glm::vec3(0.0f);
	glm::vec3 upDir = glm::vec3(0, 1, 0);
	glm::vec3 baseLookAt = -light.direction;

	glm::mat4 tempLookAt = glm::lookAt(
		zeroVector,
		baseLookAt,
		upDir
	);

	tempLookAt *= scaleMatrix;
	glm::mat4 tempInverseLookAt = glm::inverse(tempLookAt);

	center = tempLookAt * glm::vec4(center,1.0f);
	center.x = (float)floor(center.x);
	center.y = (float)floor(center.y);
	center = tempInverseLookAt * glm::vec4(center, 1.0f);

	glm::vec3 eye = center - (light.direction * radius * 2.0f);

	glm::mat4 lightView = glm::lookAt(
		eye,
		center,
		upDir
	);

	glm::mat4 lightProjection = glm::ortho(-radius, radius, -radius, radius, -radius * 6.0f, radius * 6.0f);

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
