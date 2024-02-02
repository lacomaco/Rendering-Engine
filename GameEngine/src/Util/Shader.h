#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <unordered_map>

class Shader
{
public:
	std::string readShaderSource(const char* filePath);
	GLuint compileShader(const char* shaderCode,GLenum shaderType);
	GLuint getVertexShader(const char* filePath);
	GLuint getFragmentShader(const char* filePath);
	unsigned int loadShaderProgram(
		const char* vertexShaderPath,
		const char* fragmentShaderPath,
		const char* shaderProgramName
	);

	unsigned int getShaderProgram(const char* shaderProgramName);

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
};

