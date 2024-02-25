#pragma once
#include <iostream>
#include "../Game/MeshRenderer.h"
#include "Camera.h"
#include <memory>

class Shadow
{
private:
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;

public:
	Shadow();

	void WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, Camera* camera);

	unsigned int depthMapFrameBuffer; // 그림자 생성을 위한 프레임버퍼.
	unsigned int depthMap; // 그림자 생성을 위한 텍스처.
};

