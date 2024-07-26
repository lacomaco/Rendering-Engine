#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "../Game/MeshRenderer.h"
#include <memory>
#include "./LightType.h"
#include "../GameObject/PrimitiveObject/Box.h";

using namespace std;

class CascadeShadow
{
private:
	std::vector<float> cascadeLevels;
	std::vector<glm::mat4> cacheSpaceMatrices;
	vector<glm::vec3> GetFrustumCornerWorldSpace(
		const glm::mat4& proj, const glm::mat4& view);

	vector<glm::vec3> GetFrustumCornerWorldSpace(const glm::mat4& matrix);
	void GetDirectionalLightMatrix(
		const float near, 
		const float far,
		const Light light,
		std::vector<mat4>& lightMatrices,
		std::vector<glm::mat4>& invProjMatrices,
		std::vector<float>& radiusVector,
		std::vector<mat4>& lightViewMatrices
	);

public:
	CascadeShadow(float cascadeLevel);
	void GetLightSpaceMatrices(
		Light light,
		std::vector<mat4>& lightMatrices,
		std::vector<glm::mat4>& invProjMatrices,
		std::vector<float>& radius,
		std::vector<mat4>& lightViewMatrices
	);


	std::vector<float> GetCascadeLevels() const {
		return cascadeLevels;
	}
};
