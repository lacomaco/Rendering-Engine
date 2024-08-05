#pragma once
#include <glm/glm.hpp>
#include <limits>
#include <algorithm>
#include <glew.h>

class BoundingBox
{
public:
	glm::vec3 min;
	glm::vec3 max;
	GLuint vao;
	GLuint vbo;

	BoundingBox();

	void Reset();
	void Update(const glm::vec3& point);
	bool IsIntersects(const BoundingBox& other) const;
	bool IsContains(const glm::vec3& point) const;
	bool IsContains(const BoundingBox& other) const;

	// BoundingBox 정육면체 되도록 확장
	void MakeCube();

	// 렌더링 필요할시 VAO 수동호출
	void SetupVAO();
	void Draw() const;
};

