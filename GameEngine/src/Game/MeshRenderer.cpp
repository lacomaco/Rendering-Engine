#include "MeshRenderer.h"
#include <functional>
#include <glm/glm.hpp>

void MeshRenderer::ResetMesh() {
	nonAlphaMesh.clear();
	transparentMesh.clear();
}

void MeshRenderer::MeshAlignment(Camera* camera) {
	// ���Լ�
	auto compare = [camera](const MeshTempStruct& a, const MeshTempStruct& b) {
		auto aModelPos = a.isPrimitive ? a.primitive->position : a.model->position;
		auto bModelPos = b.isPrimitive ? b.primitive->position : b.model->position;

		auto aDistance = glm::length(camera->cameraPos - a.mesh->position +aModelPos);
		auto bDistance = glm::length(camera->cameraPos - b.mesh->position + bModelPos);

		return aDistance > bDistance;
	};

	// ���� �޽� ����
	std::sort(transparentMesh.begin(), transparentMesh.end(), compare);
}

void MeshRenderer::AddMesh(std::shared_ptr<Model> model
) {
	if(model == nullptr) return;

	for (auto& mesh : model->meshes) {
		if (mesh->isAlphaMesh) {
			transparentMesh.push_back({
				mesh,
				model,
				nullptr,
				false
			});
		}
		else {
			nonAlphaMesh.push_back({
				mesh,
				model,
				nullptr,
				false
			});
		}
	}
}

void MeshRenderer::AddMesh(std::shared_ptr<Primitive> primitive) {
	if (primitive->isAlphaMesh) {
		transparentMesh.push_back({
			primitive->mesh,
			nullptr,
			primitive,
			true
		});
	}
	else {
		nonAlphaMesh.push_back({
			primitive->mesh,
			nullptr,
			primitive,
			true
		});
	}
}

void MeshRenderer::Draw(const char* programName, int instanceCount) {
	auto normalProgram = Shader::getInstance()->getShaderProgram(programName);
	glUseProgram(normalProgram);

	for (auto& meshStruct : nonAlphaMesh) {
		if (meshStruct.isPrimitive) {
			meshStruct.primitive->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName, instanceCount);
		}
		else {
			meshStruct.model->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName, instanceCount);
		}
	}

	
 	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (auto& meshStruct : transparentMesh) {
		if (meshStruct.isPrimitive) {
			meshStruct.primitive->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName, instanceCount);
		}
		else {
			meshStruct.model->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName, instanceCount);
		}
	}
	glDisable(GL_BLEND);
}