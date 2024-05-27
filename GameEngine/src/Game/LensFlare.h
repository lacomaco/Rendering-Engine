#pragma once
#include <glm/glm.hpp>
#include <glew.h>
#include "../Util/GLHelper.h"
#include "../Util/Shader.h"
#include "Bloom.h"


/*
* �˰���
* 
* 1. Flare�� DownSample�Ѵ�.
* 2. Lens Flare ����� �ִ´� Ghosting, Halo, Starbust, Streaks
* 3. Blur ó���Ѵ�.
* 4. Upsample �� �� Original Image�� ��ģ��.
*/
class LensFlare
{
private:
	unsigned int dirtLensTexture;
public:
	LensFlare();
	unsigned int lensFlareTexture;

	unsigned int vao, vbo;
	unsigned int lensFlareFBO;

	float uGhostDispersal = 0.273f;
	int uNumGhosts = 7;

	void Draw(unsigned int sceneTexture,unsigned int godRayTexture);
	void UpdateImGui();

	std::shared_ptr<Bloom> bloom;

};

