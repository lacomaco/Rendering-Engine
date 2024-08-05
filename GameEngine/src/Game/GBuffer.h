#pragma once
#include "glew.h"

/*
* ����� ���� ���� Forward Rendering���� �����ϰ� ������
* 
* SSAO ó���� ���ؼ� GBuffer�� ���� �����ϰ��ִ�.
* 
* �ð��� �Ǹ� Forward + deferred Rendering ���� �����ϴ� ������ ��������..
*/
class GBuffer
{
private:
	void createGBufferTexture(unsigned int& texture);
	void createLightingTexture();
	int currentTextureIndex = 0;
	
public:
	GBuffer();
	~GBuffer();
	// rgb: position, a: metallic
	unsigned int positionMetallicTexture;
	// rgb: albedo, a: roughness
	unsigned int albedoRoughnessTexture;
	// rgb: normal, a: �Ҵ���� ���� (0.0)
	unsigned int normalTexture;
	// godRay
	// rgba: ���� godRay�� ���ؼ� ����.
	unsigned int godRayTexture;

	unsigned int rbo;

	unsigned int gBufferFBO;

	const char* programName = "gBuffer";

	// ���� �Ű����� ������
	void use();
	void unuse();
	void UpdateImGui();
};

