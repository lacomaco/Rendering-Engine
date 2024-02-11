#pragma once
#include "VertexType.h"
#include <memory>
using namespace std;

class Primitive
{
public:
	static shared_ptr<Mesh> Plane();
	static shared_ptr<Mesh> Box(const float scale = 1.0f);
	static shared_ptr<Mesh> Sphere(
		float radius,
		int numSlices,
		int numStacks
	);

	static void CalculateTangents(
		std::vector<Vertex>& vertices,
		std::vector<unsigned int>& indices
	);
};

