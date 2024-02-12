#pragma once
#include "../VertexType.h"
#include <memory>
#include <vector>

using namespace std;

class Primitive
{
protected:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	std::shared_ptr<Mesh> mesh;

	virtual void Draw() {
		std::cout << "이거 오버라이딩 해주세요" << std::endl;
	}

	virtual void SetTexture() {
		std::cout << "이거 오버라이딩 해주세요" << std::endl;
	}

	void CalculateTangents();

public:
	~Primitive();
};

