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
* Screen Space God Rays
* 
* ȭ��� �¾��� ������ ����ȴٴ� �Ѱ����� ��Ȯ��
* 
* Volumetric God Rays (ray marching) ���� ������ �� �ִٰ��ϴµ�....
* 
* �� �𸣰���
* 
* �Ʒ� ��ƼŬ ���� deferred shading�� �ܰ��� �ϳ����� �� �۾��� �����Ѵٰ��Ѵ�.
* 
* �켱�� �׷��� ���������ο� deferred shading�� �߰��ϰ� SSAO,
* 
* �� �� �ٽ� ��������.
* 
* https://bartwronski.files.wordpress.com/2014/08/bwronski_volumetric_fog_siggraph2014.pdf
* 
* https://www.youtube.com/watch?app=desktop&v=lJi9lSe-ngE
*  Volumetric light. 
It is done as a part of the deferred rendering pipeline, 
by integrating over a shadow map in the view direction and computing 
final scattering with Henyey-Greenstein function. 
Effectively, the sun light shader determines how much light 
is scattered in the air along particular direction for each pixel, 
taking shadows into account, and then additively blends this value with 
the scene. 
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
	unsigned int godRayTexture; // ��ü�� ������ ���� �Ͼ�� �߰�
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

