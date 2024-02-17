#include "Circle.h"
#include "../../Util/stb_image.h"
#include "../../Constants.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "Primitive.h"

// hong lab의 그래픽스 수업에서 배운 내용을 바탕으로 작성하엿습니다.
Circle::Circle(float radius,
    int numSlices,
    int numStacks) {
    const float dTheta = -M_PI * 2 / float(numStacks);
    const float dPhi = -M_PI / float(numSlices);

    for (int i = 0; i <= numStacks; i++) {
        glm::vec3 stackStartPoint = glm::vec3(
            0.0f,
            -radius,
            0.0f
        );

        // Z 축 방향으로 회전 행렬
        glm::mat4 zRotationMatrix = glm::rotate(
            glm::mat4(1.0f),
            dPhi * i,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        stackStartPoint = glm::vec3(
            zRotationMatrix * glm::vec4(stackStartPoint, 1.0f)
        );


        for (int j = 0; j <= numSlices; j++) {
            Vertex v;
            glm::mat4 yRotationMatrix = glm::rotate(
                glm::mat4(1.0f),
                dTheta * j,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            v.position = glm::vec3(yRotationMatrix * glm::vec4(stackStartPoint, 1.0f));
            v.normal = glm::normalize(v.position);
            v.texcoord = glm::vec2(float(j) / numSlices, float(i) / numStacks);

            vertices.push_back(v);

        }
    }

    for (int j = 0; j < numStacks; j++) {

        const int offset = (numSlices + 1) * j;

        for (int i = 0; i < numSlices; i++) {

            indices.push_back(offset + i);
            indices.push_back(offset + i + numSlices + 1);
            indices.push_back(offset + i + 1 + numSlices + 1);

            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + numSlices + 1);
            indices.push_back(offset + i + 1);
        }
    }
}

Circle::~Circle() {
    vertices.clear();
    indices.clear();
    textures.clear();
    mesh.reset();
}

void Circle::Draw(const char* shaderProgramName) {
    PutModelUniform(shaderProgramName);
    material.PutMaterialUniforms(shaderProgramName);
    this->mesh->Draw(shaderProgramName);
}
