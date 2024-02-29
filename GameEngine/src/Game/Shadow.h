#pragma once
#include <iostream>
#include "MeshRenderer.h"
#include "Camera.h"
#include <memory>
#include "ShadowInterface.h"

class Shadow : public ShadowInterface
{
private:
	// 디버깅용도 vao
	unsigned int quadVAO;
	unsigned int quadVBO;
	std::vector<float> quadVertices;

public:
	Shadow();

	void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer,glm::mat4 lightSpaceMatrix);

	// 디버깅용도. 그림자맵 보기.
	void showDepthMap();
};

