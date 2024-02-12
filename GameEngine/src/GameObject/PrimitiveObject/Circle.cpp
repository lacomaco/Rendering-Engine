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

    SetTexture();

    CalculateTangents();

    mesh = make_shared<Mesh>(std::move(vertices), std::move(indices), std::move(textures));
    mesh->CalculateTangents();
    mesh->setupMesh();
}

Circle::~Circle() {
    vertices.clear();
    indices.clear();
    textures.clear();
    mesh.reset();
}

void Circle::Draw() {
    this->mesh->Draw();
}

void Circle::SetTexture() {
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