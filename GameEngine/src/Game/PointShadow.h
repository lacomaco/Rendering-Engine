#pragma once
#include "MeshRenderer.h"
#include <memory>
#include "glm/glm.hpp"
#include <vector>
#include "ShadowInterface.h"

class PointShadow : public ShadowInterface
{
public:
	PointShadow();

	void WriteDepthMap(
		shared_ptr<MeshRenderer> meshRenderer, 
		std::vector<glm::mat4> lightSpaceMatrix, 
		float far,
		glm::vec3 position
	);
};


