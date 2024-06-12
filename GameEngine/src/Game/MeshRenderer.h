#pragma once
#include <vector>
#include "../GameObject/MeshMaterialsLight.h"
#include "../GameObject/PrimitiveObject/Model.h"
#include "../GameObject/PrimitiveObject/Primitive.h"
#include "Camera.h"
#include <queue>

// �޽� - �� ������ �ӽ÷� �����ϴ� ����ü.
struct MeshTempStruct {
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Model> model;
	std::shared_ptr<Primitive> primitive;

	bool isPrimitive = false;
};

class MeshRenderer
{
	std::vector<MeshTempStruct> nonAlphaMesh;
	std::vector<MeshTempStruct> transparentMesh;

public:
	void ResetMesh();
	void MeshAlignment(Camera* camera);

	void AddMesh(
		std::shared_ptr<Model> model
	);

	void AddMesh(
		std::shared_ptr<Primitive> primitive
	);

	void Draw(const char* programName, int instanceCount = 0);
	
};

