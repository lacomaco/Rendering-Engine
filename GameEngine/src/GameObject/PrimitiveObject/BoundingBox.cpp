#include "BoundingBox.h"
#include <vector>

BoundingBox::BoundingBox() {
	Reset();
}

void BoundingBox::Reset() {
	min = glm::vec3(std::numeric_limits<float>::max());
	max = glm::vec3(std::numeric_limits<float>::min());
}

void BoundingBox::Update(const glm::vec3& point) {
	min.x = std::min(min.x, point.x);
	min.y = std::min(min.y, point.y);
	min.z = std::min(min.z, point.z);

	max.x = std::max(max.x, point.x);
	max.y = std::max(max.y, point.y);
	max.z = std::max(max.z, point.z);
}

bool BoundingBox::IsIntersects(const BoundingBox& other) const {
	return (min.x <= other.max.x && max.x >= other.min.x) &&
		(min.y <= other.max.y && max.y >= other.min.y) &&
		(min.z <= other.max.z && max.z >= other.min.z);
}

bool BoundingBox::IsContains(const glm::vec3& point) const {
	return (point.x >= min.x && point.x <= max.x) &&
		(point.y >= min.y && point.y <= max.y) &&
		(point.z >= min.z && point.z <= max.z);
}

bool BoundingBox::IsContains(const BoundingBox& other) const {
	return (other.min.x >= min.x && other.max.x <= max.x) &&
		(other.min.y >= min.y && other.max.y <= max.y) &&
		(other.min.z >= min.z && other.max.z <= max.z);
}

void BoundingBox::MakeCube() {
	glm::vec3 size = max - min;
	float maxSize = std::max(size.x, std::max(size.y, size.z));

	glm::vec3 center = (min + max) / 2.0f;
	glm::vec3 halfSize = glm::vec3(maxSize / 2.0f);

	min = center - halfSize;
	max = center + halfSize;
}

void BoundingBox::SetupVAO() {
	std::vector<glm::vec3> vertices = {
		// Front face
		min,
		glm::vec3(max.x, min.y, min.z),
		glm::vec3(max.x, max.y, min.z),
		glm::vec3(min.x, max.y, min.z),

		// Back face
		glm::vec3(min.x, min.y, max.z),
		glm::vec3(max.x, min.y, max.z),
		max,
		glm::vec3(min.x, max.y, max.z),

		// Connectors
		min,
		glm::vec3(min.x, min.y, max.z),
		glm::vec3(max.x, min.y, min.z),
		glm::vec3(max.x, min.y, max.z),
		glm::vec3(max.x, max.y, min.z),
		max,
		glm::vec3(min.x, max.y, min.z),
		glm::vec3(min.x, max.y, max.z)
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BoundingBox::Draw() const {
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 24); // 24 vertices to draw 12 edges of the bounding box
	glBindVertexArray(0);
}


