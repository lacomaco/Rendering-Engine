#pragma once
#include <iostream>
#include "MeshRenderer.h"
#include "Camera.h"
#include <memory>

class Shadow
{
private:
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;

	// �����뵵 vao
	unsigned int quadVAO;
	unsigned int quadVBO;
	std::vector<float> quadVertices;

public:
	Shadow();

	void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer,glm::mat4 lightSpaceMatrix);

	unsigned int depthMapFrameBuffer; // �׸��� ������ ���� �����ӹ���.
	unsigned int depthMap; // �׸��� ������ ���� �ؽ�ó.

	// �����뵵. �׸��ڸ� ����.
	void showDepthMap();
};
