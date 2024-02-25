#include "MeshRenderer.h"
#include <functional>
#include <glm/glm.hpp>

void MeshRenderer::ResetMesh() {
	nonAlphaMesh.clear();
	transparentMesh.clear();
}

void MeshRenderer::MeshAlignment(Camera* camera) {
	// 비교함수
	auto compare = [camera](const MeshTempStruct& a, const MeshTempStruct& b) {
		auto aModelPos = a.isPrimitive ? a.primitive->position : a.model->position;
		auto bModelPos = b.isPrimitive ? b.primitive->position : b.model->position;

		auto aDistance = glm::length(camera->cameraPos - a.mesh->position +aModelPos);
		auto bDistance = glm::length(camera->cameraPos - b.mesh->position + bModelPos);

		return aDistance > bDistance;
	};

	// 알파 메쉬 정렬
	std::sort(transparentMesh.begin(), transparentMesh.end(), compare);
}

void MeshRenderer::AddMesh(std::shared_ptr<Model> model
) {
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

void MeshRenderer::Draw(const char* programName) {
	auto normalProgram = Shader::getInstance()->getShaderProgram(programName);
	glUseProgram(normalProgram);

	for (auto& meshStruct : nonAlphaMesh) {
		if (meshStruct.isPrimitive) {
			meshStruct.primitive->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName);
		}
		else {
			meshStruct.model->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName);
		}
	}

	
 	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (auto& meshStruct : transparentMesh) {
		if (meshStruct.isPrimitive) {
			meshStruct.primitive->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName);
		}
		else {
			meshStruct.model->PutModelUniform(programName);
			meshStruct.mesh->Draw(programName);
		}
	}
	glDisable(GL_BLEND);
}