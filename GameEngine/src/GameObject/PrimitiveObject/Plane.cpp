#include "Plane.h"
#include "glm/glm.hpp"
#include <memory>
#include <SDL.h>
#include "../../Util/stb_image.h"


using namespace std;

// hong lab의 그래픽스 수업에서 배운 내용을 바탕으로 작성하였습니다.
Plane::Plane() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    positions.push_back(glm::vec3(-1.0f, 1.0f, 0.0f)); //0 
    positions.push_back(glm::vec3(-1.0f, -1.0f, 0.0f)); // 1
    positions.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 1.0f));
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        vertices.push_back(v);
    }

    indices = {
        0, 1, 2, 0, 2, 3,
    };
}

void Plane::Draw(const char* shaderProgramName) {
    PutModelUniform(shaderProgramName);
    this->mesh->Draw(shaderProgramName);
}

Plane::~Plane() {
    vertices.clear();
	indices.clear();
	textures.clear();
	mesh.reset();
}


