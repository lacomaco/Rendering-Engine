#pragma once
#include <vector>
#include "glm/glm.hpp"

using namespace std;

class CascadeShadow
{
private:
	vector<glm::vec4> GetFrustumCornerWorldSpace(const glm::mat4& proj, const glm::mat4& view);
public:
	CascadeShadow();
};