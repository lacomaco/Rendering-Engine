#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "../../Game/Camera.h"
#include <glew.h>
#include <stb_image.h>
#include "../../Util/Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../../Constants.h"

class CubeMap
{
private:
    void CreateCubeMapTexture(unsigned int& texture, std::vector<std::string> maps);
    void CreateBrdfLutTexture(std::string map);
    void CreateDiffuseIrradianceMap();
    void CreatePreFilterEnviromentMap();

    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f),
        1.0f, // 정사각형
        0.1f,
        10.0f
    );

    glm::mat4 captureViews[6];
    unsigned int fbo;
    unsigned int rbo;

public:
	CubeMap(std::string filePath);
    unsigned int skyBoxId;
	unsigned int preFilterEnvironmentMap;
    unsigned int irradianceId;
    unsigned int brdfLUTTextureId;
    unsigned int vao, vbo;

    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));

    void Draw(const char* shaderProgramName);

    void PutCubeMapTexture(const char* shaderProgramName);
	
    std::vector<std::string> skyBox;
    std::vector<std::string> skyBoxRadiance;

    std::vector<float> skyboxVertices = {      
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    int select = 0; // 0: skybox, 1: radiance, 2: irradiance
    float lodLevel = 0.0f;
};

