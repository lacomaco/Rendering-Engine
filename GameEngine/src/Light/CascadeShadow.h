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
	// 6.25,
	// 12.5
	// 25.0
	// 50.0
	// 100.0
	std::vector<float> cascadeLevels;
	vector<glm::vec3> GetFrustumCornerWorldSpace(
		const glm::mat4& proj, const glm::mat4& view);

	vector<glm::vec3> GetFrustumCornerWorldSpace(const glm::mat4& matrix);
	glm::mat4 GetDirectionalLightMatrix(const float near, 
		const float far,
		const Light light);

	// for Debug
	std::vector<GLuint> visualizerVAOs;
	std::vector<GLuint> visualizerVBOs;
	std::vector<GLuint> visualizerEBOs;

public:
	CascadeShadow(float cascadeLevel);
	std::vector<glm::mat4> GetLightSpaceMatrices(Light light);


	std::vector<float> GetCascadeLevels() const {
		return cascadeLevels;
	}

	void drawCascadeVolumeVisualizers(Light light);
};
