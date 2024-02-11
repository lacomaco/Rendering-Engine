#pragma once
#include "../VertexType.h"
#include <vector>
#include <memory>


class Plane
{
public:
	Plane();
	~Plane();
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	std::shared_ptr<Mesh> mesh;
	void Draw();
	void SetTexture();

};

