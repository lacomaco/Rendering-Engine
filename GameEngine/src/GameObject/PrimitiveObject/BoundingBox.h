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

	// BoundingBox ������ü �ǵ��� Ȯ��
	void MakeCube();

	// ������ �ʿ��ҽ� VAO ����ȣ��
	void SetupVAO();
	void Draw() const;
};
