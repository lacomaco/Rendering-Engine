#include "CubeMap.h"
#include <memory>
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
    
    std::vector<std::string> skyBoxRadiance = {
        filePath + "radiance_posx.dds",
        filePath + "radiance_negx.dds",
        filePath + "radiance_posy.dds",
        filePath + "radiance_negy.dds",
        filePath + "radiance_posz.dds",
        filePath + "radiance_negz.dds"
    };

    CreateCubeMapTexture(skyBoxId, skyBox);
    CreateCubeMapTexture(preFilterEnvironmentMap, skyBoxRadiance);
    // TODO: test 말고 envbrdf.dds로 바꾸자.
    CreateBrdfLutTexture(filePath + "test.dds");


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


    CreateDiffuseIrradianceMap();
}


void CubeMap::CreateBrdfLutTexture(std::string map) {
    glGenTextures(1, &brdfLUTTextureId);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    gli::texture2d Texture(gli::load_dds(map.c_str()));
    gli::flip(Texture);

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const Format(GL.translate(Texture.format(), Texture.swizzles()));
    
    glTexImage2D(GL_TEXTURE_2D,
        0,
        Format.Internal,
        Texture.extent().x,
        Texture.extent().y,
       	0,
        Format.External,
        Format.Type,
        Texture.data()
    );
}

void CubeMap::CreateCubeMapTexture(unsigned int& texture, std::vector<std::string> maps) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 8);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "Error Bind Texture : " << error << std::endl;
    }

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

    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "mipmapFalse : " << error << std::endl;
    }
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceId);
    shader->setInt(shaderProgramName, "irradianceMap", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTextureId);
    shader->setInt(shaderProgramName, "brdfTexture", 3);

    shader->setFloat(shaderProgramName, "lodLevel", lodLevel);
    shader->setInt(shaderProgramName, "select", select);
}

void CubeMap::Draw(const char* shaderProgramName) {
    auto shader = Shader::getInstance();
    glUseProgram(shader->getShaderProgram(shaderProgramName));
    PutCubeMapTexture(shaderProgramName);
    shader->setMat4(shaderProgramName, "model", scaleMatrix);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CubeMap::CreateDiffuseIrradianceMap() {
    glGenTextures(1, &irradianceId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceId);

    const int textureSize = 128;

    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB16F,
            textureSize,
            textureSize,
            0,
            GL_RGB,
            GL_FLOAT,
            nullptr
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, textureSize, textureSize);

    auto shader = Shader::getInstance();
    const char* shaderProgramName = "diffuse-irradiance";
    glUseProgram(shader->getShaderProgram("diffuse-irradiance"));
    shader->setInt(shaderProgramName,"skyMap",0);
    shader->setMat4(shaderProgramName,"projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);

    glViewport(0, 0, textureSize, textureSize);
    for (unsigned int i= 0; i < 6; i++) {
        shader->setMat4(shaderProgramName, "view", captureViews[i]);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            irradianceId,
            0
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void CubeMap::CreatePreFilterEnviromentMap() {
    glGenTextures(1, &preFilterEnvironmentMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, preFilterEnvironmentMap);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB16F,
            128,
            128,
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
}
