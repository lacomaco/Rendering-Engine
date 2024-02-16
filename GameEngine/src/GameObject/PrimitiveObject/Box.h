#pragma once
#include "../MeshMaterialsLight.h"
#include "Primitive.h"
#include <vector>
#include <memory>

class Box : public Primitive
{
public:
	Box();
	~Box();
	void Draw(const char* shaderProgramName);
	void SetTexture(std::string path, std::string type);
	// SetupMesh 호출 후 Draw 해주세요.
	void SetupMesh();
};

