#pragma once
#include "../VertexType.h"
#include "Primitive.h"
#include <vector>
#include <memory>

class Box : public Primitive
{
public:
	Box();
	~Box();
	void Draw(const char* shaderProgramName);
	void SetTexture();
};

