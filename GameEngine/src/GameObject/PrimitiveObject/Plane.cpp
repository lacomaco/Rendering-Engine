#include "Plane.h"
#include "glm/glm.hpp"
#include <memory>

using namespace std;

Plane::Plane() {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    positions.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
    positions.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    texcoords.push_back(glm::vec2(0.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f));
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        vertices.push_back(v);
    }

    indices = {
        0, 1, 2, 0, 2, 3,
    };

    mesh = make_shared<Mesh>(std::move(vertices), std::move(indices));
    mesh->CalculateTangents();
    mesh->setupMesh();
}

void Plane::Draw() {
    this->mesh->Draw();
}

Plane::~Plane() {
}


