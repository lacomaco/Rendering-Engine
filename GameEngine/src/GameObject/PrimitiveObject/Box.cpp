#include "Box.h"
#include "glm/glm.hpp"
#include "../../Util/stb_image.h"
#include "../MeshMaterialsLight.h"


using namespace std;

// hong lab의 그래픽스 수업에서 배운 내용을 바탕으로 작성하엿습니다.
Box::Box() {
    vector<glm::vec3> positions;
    vector<glm::vec3> colors;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    // 윗면
    positions.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));
    positions.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, -1.0f));
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

    // 아랫면
    positions.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, -1.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, 1.0f));
    positions.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));
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

    // 뒷면
    positions.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
    positions.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, -1.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, -1.0f));
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

    // 앞면
    positions.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, 1.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    positions.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
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

    // 왼쪽
    positions.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));
    positions.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
    positions.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));
    positions.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
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

    // 오른쪽
    positions.push_back(glm::vec3(1.0f, -1.0f, 1.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, -1.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, -1.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
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

    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        vertices.push_back(v);
    }

    indices = {
        0,  1,  2,  0,  2,  3,  // 윗면
        4,  5,  6,  4,  6,  7,  // 아랫면
        8,  9,  10, 8,  10, 11, // 앞면
        12, 13, 14, 12, 14, 15, // 뒷면
        16, 17, 18, 16, 18, 19, // 왼쪽
        20, 21, 22, 20, 22, 23  // 오른쪽
    };
}

void Box::SetupMesh() {
    CalculateTangents();

	mesh = make_shared<Mesh>(std::move(vertices), std::move(indices), std::move(textures));
	mesh->CalculateTangents();
	mesh->setupMesh();
}

void Box::Draw(const char* shaderProgramName) {
    PutModelUniform(shaderProgramName);
    material.PutMaterialUniforms(shaderProgramName);
    this->mesh->Draw(shaderProgramName);
}

void Box::SetTexture(std::string path, std::string type) {
    int width, height, nrChannels;

    Texture texture;
    texture.id = 0;
    texture.type = type;
    texture.path = path;

    unsigned char* data = stbi_load(texture.path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture" << std::endl;
    }

    GLenum format;

    if (nrChannels == 1) {
		format = GL_RED;
	}
    else if (nrChannels == 3) {
		format = GL_RGB;
	}
    else if (nrChannels == 4) {
		format = GL_RGBA;
	}

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(
        GL_TEXTURE_2D, // 텍스처 바인딩 대상 지정, 큐브맵은 GL_TEXTURE_3D이다.
        0, // mipmap 레벨
        format, // internal formap
        width,
        height,
        0, // border 크기
        format, // 입력 데이터 형식
        GL_UNSIGNED_BYTE, // 픽셀 데이터 타입
        data // 픽셀 데이터 포인터.
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    /*
    * S: 수평축 (x축)
    * T: 수직축 (y축)
    * 특별한 약어가 있는것은 아니다. 그냥 전통적인 명명법이라한다.
    */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    textures.push_back(texture);
}

Box::~Box() {
    vertices.clear();
    indices.clear();
    textures.clear();
    mesh.reset();
}


