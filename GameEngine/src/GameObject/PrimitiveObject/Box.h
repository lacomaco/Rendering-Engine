#pragma once
#include "../VertexType.h"
#include "Primitive.h"
#include <vector>
#include <memory>

class Box : public Primitive
{
public:
	Box(float scale = 1.0f);
	~Box();
	void Draw();
	void SetTexture();
};

