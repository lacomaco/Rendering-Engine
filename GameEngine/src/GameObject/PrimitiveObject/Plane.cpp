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
    positions.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
    positions.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
    positions.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
    positions.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    texcoords.push_back(glm::vec2(0.0f, 1.0f)); // 좌하단
    texcoords.push_back(glm::vec2(1.0f, 1.0f)); // 우하단
    texcoords.push_back(glm::vec2(1.0f, 0.0f)); // 우상단
    texcoords.push_back(glm::vec2(0.0f, 0.0f)); // 좌상단
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
    SetTexture();
    CalculateTangents();
    mesh = make_shared<Mesh>(std::move(vertices), std::move(indices),std::move(textures));
    mesh->setupMesh();
}

void Plane::Draw(const char* shaderProgramName) {
    PutModelUniform(shaderProgramName);
    this->mesh->Draw();
}

void Plane::SetTexture() {
    int width, height, nrChannels;

	Texture texture;
	texture.id = 0;
	texture.type = "albedo";
	texture.path = "./assets/images/wall.jpg";

    unsigned char* data = stbi_load(texture.path.c_str(), &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    /*
    * S: 수평축 (x축)
    * T: 수직축 (y축)
    * 특별한 약어가 있는것은 아니다. 그냥 전통적인 명명법이라한다.
    */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D, // 텍스처 바인딩 대상 지정, 큐브맵은 GL_TEXTURE_3D이다.
        0, // mipmap 레벨
        GL_RGB, // internal formap
        width,
        height,
        0, // border 크기
        GL_RGB, // 입력 데이터 형식
        GL_UNSIGNED_BYTE, // 픽셀 데이터 타입
        data // 픽셀 데이터 포인터.
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    Texture texture2;
    texture2.id = 0;
    texture2.type = "albedo";
    texture2.path = "./assets/images/awesomeface.png";

    glGenTextures(1, &texture2.id);
    glBindTexture(GL_TEXTURE_2D, texture2.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    unsigned char* data2 = stbi_load(texture2.path.c_str(), &width, &height, &nrChannels, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    /*
    * mipmap? : 텍스처 매핑 기법.
    * GPU에서 텍스처를 미리 절반으로 줄인 작은 사이즈의 텍스처를 만들어둔다.
    * ex) 256* 256 -> 128 * 128 -> 64 * 64 -> 32 * 32 -> 16 * 16 -> 8 * 8 -> 4 * 4 -> 2 * 2 -> 1 * 1
    * mipmap level 역시 이에 매칭된다.
    * mipmap 0 -> 256 * 256
    * mipmap 1 -> 128 * 128
    *
    * 만약 거리가 먼 물체를 렌더링할때 굳이 해상도가 높은 텍스처를 쓸 필요가 없으니
    mipmap을 적절히 조절하면 최적화와 시각적 품질을 얻을 수 있다.
    */
    stbi_image_free(data);
    stbi_image_free(data2);



	textures.push_back(texture);
    textures.push_back(texture2);
}

Plane::~Plane() {
    vertices.clear();
	indices.clear();
	textures.clear();
	mesh.reset();
}


