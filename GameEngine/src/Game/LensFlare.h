#pragma once
#include <glm/glm.hpp>
#include <glew.h>
#include "../Util/GLHelper.h"
#include "../Util/Shader.h"
#include "Bloom.h"


/*
* 알고리즘
* 
* 1. Flare를 DownSample한다.
* 2. Lens Flare 기능을 넣는다 Ghosting, Halo, Starbust, Streaks
* 3. Blur 처리한다.
* 4. Upsample 한 후 Original Image와 합친다.
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

