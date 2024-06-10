#include "CascadeShadow.h"

CascadeShadow::CascadeShadow() {

}

vector<glm::vec4> CascadeShadow::GetFrustumCornerWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
	const glm::mat4 inverseProjectionView = glm::inverse(proj * view);

	std::vector<glm::vec4> frustumCorners;

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				glm::vec4 point = glm::vec4(x*2.0f - 1.0f, y*2.0f - 1.0f, z*2.0f - 1.0f, 1.0f);
				glm::vec4 worldPoint = inverseProjectionView * point;
				worldPoint = worldPoint / worldPoint.w;

				frustumCorners.push_back(worldPoint);
			}
		}
	}

	return frustumCorners;
}
