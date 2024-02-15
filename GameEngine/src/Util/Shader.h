#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader
{
public:
	std::string readShaderSource(const char* filePath);
	GLuint compileShader(const char* shaderCode,GLenum shaderType);
	GLuint getVertexShader(const char* filePath);
	GLuint getFragmentShader(const char* filePath);
	GLuint getGeometryShader(const char* filePath);
	unsigned int loadShaderProgram(
		const char* vertexShaderPath,
		const char* fragmentShaderPath,
		const char* shaderProgramName,
		const char* geometryShaderPath = nullptr
	);

	unsigned int getShaderProgram(const char* shaderProgramName);

	void setBool(const char* shaderProgramName, const char* name, bool value);
	void setInt(const char* shaderProgramName, const char* name, int value);
	void setFloat(const char* shaderProgramName, const char* name, float value);
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

	static Shader* instance;
	std::unordered_map<const char*, GLuint> shaderMap;
	std::unordered_map<const char*, int> shaderProgramMap;

	std::string ProcessShaderIncludes(const std::string& source, const std::string& directoryPath);
};

