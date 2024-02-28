#pragma once
#include <memory>
#include "MeshRenderer.h"
#include "glm/glm.hpp"

class ShadowInterface {
private:
	const unsigned int SHADOW_WIDTH = 2048 * 2;
	const unsigned int SHADOW_HEIGHT = 2048 * 2;

public:
	virtual void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, glm::mat4 lightSpaceMatrix) {
		std::cerr << "일반 그림자 오버로딩 해주세용..." << std::endl;
	}

	virtual void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, std::vector<glm::mat4> lightSpaceMatrix) {
		std::cerr << "포인트 그림자 오버로딩 해주세용..." << std::endl;
	}


};
