#pragma once
#include "glew.h"

/*
* 현재는 메인 씬을 Forward Rendering으로 구현하고 있지만
* 
* SSAO 처리를 위해서 GBuffer도 같이 생성하고있다.
* 
* 시간이 되면 Forward + deferred Rendering 둘을 병행하는 구조로 변경하자..
*/
class GBuffer
{
private:
	void createGBufferTexture(unsigned int& texture);
	int currentTextureIndex = 0;
	
public:
	GBuffer();
	~GBuffer();
	// rgb: position, a: metallic
	unsigned int positionMetallicTexture;
	// rgb: albedo, a: roughness
	unsigned int albedoRoughnessTexture;
	// rgb: normal, a: 할당되지 않음 (0.0)
	unsigned int normalTexture;
	// godRay
	// rgba: 전부 godRay를 위해서 사용됨.
	unsigned int godRayTexture;

	unsigned int rbo;

	unsigned int gBufferFBO;

	const char* programName = "gBuffer";

	// 아직 매개변수 못정함
	void use();
	void unuse();
	void UpdateImGui();
};

