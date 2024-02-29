#pragma once
#include <memory>
#include "MeshRenderer.h"
#include "glm/glm.hpp"

class ShadowInterface {
public:
	const unsigned int SHADOW_WIDTH = 2048 * 2;
	const unsigned int SHADOW_HEIGHT = 2048 * 2;

	virtual void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, glm::mat4 lightSpaceMatrix) {
		std::cerr << "일반 그림자 오버로딩 해주세용..." << std::endl;
	}

	virtual void WriteDepthMap(
		shared_ptr<MeshRenderer> meshRenderer, 
		std::vector<glm::mat4> lightSpaceMatrix,
		float far,
		glm::vec3 position
	) {
		std::cerr << "포인트 그림자 오버로딩 해주세용..." << std::endl;
	}

	// direcitonal shader용도
	unsigned int depthMapFrameBuffer; // 그림자 생성을 위한 프레임버퍼.
	unsigned int depthMap; // 그림자 생성을 위한 텍스처.

	// 큐브맵 쉐이더 용도.
	unsigned int depthCubeMap;
	unsigned int depthMapFBO;

	unsigned int getDepthMapTexture(int lightType) {
		if (lightType == 1) {
			return depthCubeMap;
		}

		return depthMap;
	}

	unsigned int getDepthMapFrameBuffer(int lightType) {
		if (lightType == 1) {
			return depthMapFBO;
		}

		return depthMapFrameBuffer;
	}


};
