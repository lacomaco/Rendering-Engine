#include "EditorSharedValue.h"

namespace EditorSharedValue {
	unsigned int EditorMainSceneFrameBuffer = 0;
	bool editorMode = true;
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	bool showNormal = false;
	float cameraSpeed = 2.5f;
	float exposure = 1.0f;

	glm::vec3 directionalLightPosition = glm::vec3(0, 20, -0.001);
	glm::vec3 directionalLightDirection = glm::vec3(0, -1, -0.221);
	unsigned int directionalLightDepthMap = 0;

	float bloomThreshold = 0.04;

	int select = 0; // 0: skybox, 1: radiance, 2: irradiance
	float lodLevel = 0.0f;

	float metallic = 0.78f;
	float roughness = 0.0f;

	bool useSun = true;
	bool usePointLight = false;

	unsigned int godLightTexture = 0;
	float decay = 0.0f;
	float density = 0.0f;
	float weight = 0.0f;
	float godRayExposure = 0.0f;

	unsigned int positionMetallicTexture = 0;
	unsigned int albedoRoughnessTexture = 0;
	unsigned int normalTexture = 0;
	unsigned int godRayTexture = 0;

	unsigned int ssaoTexture = 0;
	unsigned int ssaoBlurTexture = 0;

	float radius = 0.5f;
	float bias = 0.025f;
	bool useSSAO = true;

	unsigned int lensFlareTexture = 0;
	int uGhosts = 3;
	float uGhostDispersal = 0.4f;
}