#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "../Game/MeshRenderer.h"
#include <memory>
#include "./LightType.h"

using namespace std;

class CascadeShadow
{
private:
	std::vector<float> cascadeLevels;
	vector<glm::vec4> GetFrustumCornerWorldSpace(
		const glm::mat4& proj, const glm::mat4& view);
	glm::mat4 GetDirectionalLightMatrix(const float near, 
		const float far,
		const Light light);
	glm::mat4 GetDirectionalLightMatrixTemp(const float near,
		const float far,
		const Light light);
public:
	CascadeShadow(float cascadeLevel);
	std::vector<glm::mat4> GetLightSpaceMatrices(Light light);


	std::vector<float> GetCascadeLevels() const {
		return cascadeLevels;
	}
};
