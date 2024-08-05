#pragma once
#include <glm/glm.hpp>

namespace EditorSharedValue {

	extern unsigned int EditorMainSceneFrameBuffer;
	extern bool editorMode;
	extern glm::vec3 cameraPosition;
	extern glm::vec3 cameraFront;
	extern bool showNormal;
	extern float cameraSpeed;
	extern float exposure;

	// directional light 관련
	extern glm::vec3 directionalLightPosition;
	extern glm::vec3 directionalLightDirection;
	extern unsigned int directionalLightDepthMap;

	// Bloom 관련
	extern float bloomThreshold;

	// cube맵 관련
	extern int select; // 0: skybox, 1: radiance, 2: irradiance
	extern float lodLevel;

	// PBR 테스트용.
	// TODO: 나중에 지우기.
	extern float metallic;
	extern float roughness;

	// 라이트
	extern bool useSun;
	extern bool usePointLight;

	// god light
	extern unsigned int godLightTexture;
	extern float decay;
	extern float density;
	extern float weight;
	extern float godRayExposure;

	// GBUFFER 텍스처 시각화.
	extern unsigned int positionMetallicTexture;
	extern unsigned int albedoRoughnessTexture;
	extern unsigned int normalTexture;
	extern unsigned int godRayTexture;
	extern unsigned int lightingTexture;

	// SSAO
	extern unsigned int ssaoTexture;
	extern unsigned int ssaoBlurTexture;

	extern float radius;
	extern float bias;
	extern bool useSSAO;

	// Lens Flare
	extern unsigned int lensFlareTexture;
	extern int uGhosts;
	extern float uGhostDispersal;
}
