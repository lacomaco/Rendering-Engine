#include "Primitive.h"
#include "../Constants.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

// 하드코딩 나중에 블렌더로 아예 Plane 사각형 만들어서 로드하는 방식으로 바꿔도 나쁘지 않을듯
shared_ptr<Mesh> Primitive::Plane() {
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
    vector<Vertex>  vertices;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        vertices.push_back(v);
    }
    vector<unsigned int> indices = {
		0, 1, 2, 0, 2, 3,
	};
    Primitive::CalculateTangents(vertices, indices);

    return make_shared<Mesh>(std::move(vertices), std::move(indices));
}

// 하드코딩 나중에 블렌더로 아예 Plane 사각형 만들어서 로드하는 방식으로 바꿔도 나쁘지 않을듯
shared_ptr<Mesh> Primitive::Box(const float scale) {
    vector<glm::vec3> positions;
    vector<glm::vec3> colors;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    positions.push_back(glm::vec3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, 1.0f, -1.0f) * scale);
    colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    texcoords.push_back(glm::vec2(0.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f));
    positions.push_back(glm::vec3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(-1.0f, -1.0f, 1.0f) * scale);
    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    texcoords.push_back(glm::vec2(0.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f));
    positions.push_back(glm::vec3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, -1.0f, -1.0f) * scale);
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
    positions.push_back(glm::vec3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(-1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f));
    positions.push_back(glm::vec3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(-1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    texcoords.push_back(glm::vec2(0.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(glm::vec3(1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    texcoords.push_back(glm::vec2(0.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 0.0f));
    texcoords.push_back(glm::vec2(1.0f, 1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f));
    vector<Vertex>  vertices;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        vertices.push_back(v);
    }

    vector<unsigned int> indices = {
		0,  1,  2,  0,  2,  3,  // 윗면
		4,  5,  6,  4,  6,  7,  // 아랫면
		8,  9,  10, 8,  10, 11, // 앞면
		12, 13, 14, 12, 14, 15, // 뒷면
		16, 17, 18, 16, 18, 19, // 왼쪽
		20, 21, 22, 20, 22, 23  // 오른쪽
	};

    Primitive::CalculateTangents(vertices, indices);

    return make_shared<Mesh>(std::move(vertices), std::move(indices));
}

// https://www.songho.ca/opengl/gl_sphere.html#example_sphere
shared_ptr<Mesh> Primitive::Sphere(
    float radius,
    int numSlices,
    int numStacks
) {
    const float dTheta =  -M_PI *2 / float(numStacks);
    const float dPhi = -M_PI / float(numSlices);
    
    vector <Vertex> vertices;

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
            v.texcoord = glm::vec2(float(j) / numSlices, 1.0f - float(i) / numStacks);

            vertices.push_back(v);

        }
    }

    vector<unsigned int> indices;

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

    Primitive::CalculateTangents(vertices, indices);

    return make_shared<Mesh>(std::move(vertices), std::move(indices));
}

// 지울거임.
void Primitive::CalculateTangents(
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices
) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        // Tangent 구하는 공식.
        // see here : https://learnopengl.com/Advanced-Lighting/Normal-Mapping
		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		float deltaU1 = v1.texcoord.x - v0.texcoord.x;
		float deltaV1 = v1.texcoord.y - v0.texcoord.y;
		float deltaU2 = v2.texcoord.x - v0.texcoord.x;
		float deltaV2 = v2.texcoord.y - v0.texcoord.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		glm::vec3 tangent;
		tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

        // vertex가 공유되는 경우 평균으로 처리한다.
		v0.tangentModel += tangent;
		v1.tangentModel += tangent;
		v2.tangentModel += tangent;
	}

    for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].tangentModel = glm::normalize(vertices[i].tangentModel);
	}
}
