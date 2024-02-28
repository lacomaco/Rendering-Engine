#pragma once
#include <iostream>
#include "MeshRenderer.h"
#include "Camera.h"
#include <memory>
#include "ShadowInterface.h"

class Shadow : public ShadowInterface
{
private:
	const unsigned int SHADOW_WIDTH = 2048 * 2;
	const unsigned int SHADOW_HEIGHT = 2048 * 2;

	// 디버깅용도 vao
	unsigned int quadVAO;
	unsigned int quadVBO;
	std::vector<float> quadVertices;

public:
	Shadow();

	void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer,glm::mat4 lightSpaceMatrix);

	unsigned int depthMapFrameBuffer; // 그림자 생성을 위한 프레임버퍼.
	unsigned int depthMap; // 그림자 생성을 위한 텍스처.

	// 디버깅용도. 그림자맵 보기.
	void showDepthMap();
};

