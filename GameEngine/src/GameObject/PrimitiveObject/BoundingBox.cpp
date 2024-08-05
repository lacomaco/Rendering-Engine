#include "BoundingBox.h"

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


