#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "MeshRenderer.h"
#include "../Util//GLHelper.h"
#include "Camera.h"
#include <memory>
#include "../GameObject/PrimitiveObject/Circle.h"
#include "LensFlare.h"

/*
* 
* https://bartwronski.files.wordpress.com/2014/08/bwronski_volumetric_fog_siggraph2014.pdf
* 
* https://ijdykeman.github.io/graphics/simple_fog_shader
* 
*  Volumetric light
*/
class GodRays
{
private:
	std::vector<float> quad = quadVertices;
public:
	GodRays();
	~GodRays();

	void Draw(
		shared_ptr<MeshRenderer> mesh,
		glm::vec3 rayPosition,
		std::shared_ptr<Camera> camera,
		unsigned int godRaySceneTexture
	);
	unsigned int godRayTexture; // 물체는 검정색 빛만 하얀색 추가
	unsigned int godRayFrameBuffer;
	unsigned int rbo;

	unsigned int vao;
	unsigned int vbo;

	float decay = 0.985;
	float density = 1;
	float weight = 1;
	float exposure = 0.384;

	std::shared_ptr<Circle> rayCircle;

	void ImGuiUpdate();
};

