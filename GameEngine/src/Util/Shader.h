#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glew.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glew.h>

class Shader
{
public:
	std::string readShaderSource(const char* filePath);
	GLuint compileShader(const char* shaderCode,GLenum shaderType, const char* filePath);
	unsigned int loadShaderProgram(
		const char* vertexShaderPath,
		const char* fragmentShaderPath,
		const char* shaderProgramName,
		const char* geometryShaderPath = nullptr
	);

	unsigned int loadComputeShaderProgram(
		const char* computeShaderPath,
		const char* shaderProgramName
	);

	unsigned int getShaderProgram(const char* shaderProgramName);

	void setBool(const char* shaderProgramName, const char* name, bool value);
	void setInt(const char* shaderProgramName, const char* name, int value);
	void setFloat(const char* shaderProgramName, const char* name, float value);
	void setFloat(const char* shaderProgramName, const char* name, float* values, int count);
	void setVec2(const char* shaderProgramName, const char* name, const glm::vec2& value);
	void setVec2(const char* shaderProgramName, const char* name, float x, float y);
	void setVec3(const char* shaderProgramName, const char* name, const glm::vec3& value);
	void setVec3(const char* shaderProgramName, const char* name, float x, float y, float z);
	void setVec4(const char* shaderProgramName, const char* name, const glm::vec4& value);
	void setVec4(const char* shaderProgramName, const char* name, float x, float y, float z, float w);
	void setMat2(const char* shaderProgramName, const char* name, const glm::mat2& mat);
	void setMat3(const char* shaderProgramName, const char* name, const glm::mat3& mat);
	void setMat4(const char* shaderProgramName, const char* name, const glm::mat4& mat);


	static Shader* getInstance() {
		if (instance == nullptr) {
			instance = new Shader();
		}
		return instance;
	}

private:
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader() = default;
	~Shader() = default;

	GLuint getShader(const char* filePath, GLenum shaderType);

	static Shader* instance;
	std::unordered_map<const char*, GLuint> shaderMap;
	std::unordered_map<const char*, int> shaderProgramMap;

	std::string ProcessShaderIncludes(const std::string& source, const std::string& directoryPath);
};

