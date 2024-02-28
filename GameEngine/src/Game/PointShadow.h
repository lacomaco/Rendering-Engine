#pragma once
#include "MeshRenderer.h"
#include <memory>
#include "glm/glm.hpp"
#include <vector>
#include "ShadowInterface.h"

class PointShadow : public ShadowInterface
{

private:
	const unsigned int SHADOW_WIDTH = 2048 * 2;
	const unsigned int SHADOW_HEIGHT = 2048 * 2;

public:
	PointShadow();
	unsigned int depthCubeMap;
	unsigned int depthMapFBO;

	void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, std::vector<glm::mat4> lightSpaceMatrix);
};


