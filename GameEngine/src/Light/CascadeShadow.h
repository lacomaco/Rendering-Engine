#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "../Game/MeshRenderer.h"
#include <memory>

using namespace std;

class CascadeShadow
{
private:
	std::vector<float> cascadeLevels;
	vector<glm::vec4> GetFrustumCornerWorldSpace(const glm::mat4& proj, const glm::mat4& view);
	glm::mat4 GetDirectionalLightMatrix(const float near, const float far, const glm::vec3 lightDirection);
public:
	CascadeShadow(float cascadeLevel);
	std::vector<glm::mat4> GetLightSpaceMatrices(glm::vec3 lightDirection);

	int CreateShadow(
		shared_ptr<MeshRenderer> meshRenderer,
		glm::vec3 lightDirection,
		unsigned int fbo,
		unsigned int textureArray,
		unsigned int index
	);
};