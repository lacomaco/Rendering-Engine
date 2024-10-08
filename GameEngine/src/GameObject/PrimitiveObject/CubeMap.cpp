#include "CubeMap.h"
#include <memory>
#include "../../Util/GLHelper.h"
#include <gli/gli.hpp>

CubeMap::CubeMap(std::string filePath) {

    captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    std::vector<std::string> skyBox = {
        filePath + "skybox_posx.dds",
        filePath + "skybox_negx.dds",
        filePath + "skybox_posy.dds",
        filePath + "skybox_negy.dds",
        filePath + "skybox_posz.dds",
        filePath + "skybox_negz.dds"
    };

    CreateCubeMapTexture(skyBoxId, skyBox);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, 
        skyboxVertices.size() * sizeof(float), 
        skyboxVertices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenRenderbuffers(1, &rbo);
    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    CreatePreFilterEnviromentMap();
    CreateBRDFLut();
    CalculateSHCoefficients();
}

void CubeMap::CreateCubeMapTexture(unsigned int& texture, std::vector<std::string> maps) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 8);

    for (unsigned int i = 0; i < maps.size(); i++)
    {
        gli::texture2d Texture(gli::load_dds(maps[i].c_str()));
        gli::flip(Texture);
        gli::gl GL(gli::gl::PROFILE_GL33);
        gli::gl::format const Format(GL.translate(Texture.format(), Texture.swizzles()));

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB32F,
            Texture.extent().x,
            Texture.extent().y,
            0,
            GL_RGBA,
            GL_FLOAT,
            Texture.data()
        );

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "Error loading texture: " << error << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void CubeMap::CalculateSHCoefficients() {
    std::vector<float> a1Data(9 * 3);

    GLuint a1;
    glGenBuffers(1, &a1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, a1);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER, 
        a1Data.size() * sizeof(float), 
        a1Data.data(), 
        GL_DYNAMIC_COPY
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, a1);

    auto shader = Shader::getInstance();

    glUseProgram(shader->getShaderProgram("irradiance"));
    shader->setInt("irradiance", "skyMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);

    glDispatchCompute(1, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, a1);
    glGetBufferSubData(
        GL_SHADER_STORAGE_BUFFER, 
        0, 
        a1Data.size() * sizeof(float),
        a1Data.data()
    );


    std::cout << "Compute Shade Result!";
    for (int i = 0; i < a1Data.size(); i++) {
        std::cout << a1Data[i] << " ";
    }
    std::cout << "Compute Shade Done!";

    SHCoeffs = a1Data;
}

void CubeMap::PutCubeMapTexture(const char* shaderProgramName) {
	auto shader = Shader::getInstance();
	glUseProgram(shader->getShaderProgram(shaderProgramName));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);
    shader->setInt(shaderProgramName, "skyBox", 0);

    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, preFilterEnvironmentMap);
    shader->setInt(shaderProgramName, "preFilterEnvironmentMap", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTextureId);
    shader->setInt(shaderProgramName, "brdfTexture", 2);

    shader->setFloat(shaderProgramName, "lodLevel", lodLevel);
    shader->setInt(shaderProgramName, "select", select);
    shader->setFloat(shaderProgramName, "shCoeffs", SHCoeffs.data(), SHCoeffs.size());
}

void CubeMap::Draw(const char* shaderProgramName) {
    auto shader = Shader::getInstance();
    glUseProgram(shader->getShaderProgram(shaderProgramName));
    PutCubeMapTexture(shaderProgramName);
    shader->setMat4(shaderProgramName, "model", scaleMatrix);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CubeMap::CreatePreFilterEnviromentMap() {
    glGenTextures(1, &preFilterEnvironmentMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, preFilterEnvironmentMap);
    const int cubeMapSize = 256;
    for (int i = 0; i < 6; i++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB16F,
            cubeMapSize,
            cubeMapSize,
            0,
            GL_RGB,
            GL_FLOAT,
            nullptr
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    auto shader = Shader::getInstance();
    const char* shaderProgramName = "pre-filter-environment";
    glUseProgram(shader->getShaderProgram(shaderProgramName));
    shader->setInt(shaderProgramName, "skyMap", 0);
    shader->setMat4(shaderProgramName, "projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    unsigned int maxMipLevel = 9;
    for (unsigned int mip = 0; mip < maxMipLevel; mip++) {
        const int mipWidth = cubeMapSize * std::pow(0.5, mip);
        const int mipHeight = cubeMapSize * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        const float roughness = (float)mip / (float)(maxMipLevel - 1);
        for (int i = 0; i < 6; i++) {
            shader->setMat4(shaderProgramName, "view", captureViews[i]);
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                preFilterEnvironmentMap,
                mip
            );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void CubeMap::CreateBRDFLut() {
    glGenTextures(1, &brdfLUTTextureId);

    glBindTexture(GL_TEXTURE_2D, brdfLUTTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SimpleQuad quad = CreateSimpleQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTextureId, 0);

    glViewport(0, 0, 512, 512);
    auto shader = Shader::getInstance();
    glUseProgram(shader->getShaderProgram("brdf-lut"));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(quad.VAO);
    glDrawArrays(GL_TRIANGLES,0,6);
}
