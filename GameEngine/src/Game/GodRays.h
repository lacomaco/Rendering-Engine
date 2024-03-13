#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "MeshRenderer.h"
#include "../Util//GLHelper.h"
#include "Camera.h"
#include "Light.h"
#include <memory>
#include "../GameObject/PrimitiveObject/Circle.h"

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
		std::shared_ptr<Camera> camera
	);
	unsigned int godRaySceneTexture;
	unsigned int godRayTexture; // 물체는 검정색 빛만 하얀색 추가
	unsigned int godRayFrameBuffer;
	unsigned int rbo;

	unsigned int vao;
	unsigned int vbo;

	std::shared_ptr<Circle> rayCircle;
};

