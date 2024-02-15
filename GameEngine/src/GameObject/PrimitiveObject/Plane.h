#pragma once
#include "../MeshMaterialsLight.h"
#include <vector>
#include <memory>
#include "Primitive.h"

class Plane : public Primitive
{
public:
	Plane();
	~Plane();
	void Draw(const char* shaderProgramName);
	void SetTexture();
};

