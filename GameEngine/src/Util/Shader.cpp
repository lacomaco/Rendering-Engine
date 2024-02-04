#include "Shader.h"

Shader* Shader::instance = nullptr;

std::string Shader::readShaderSource(const char* filePath)
{
	std::ifstream shaderFile;
    /*
    * failbit <- I/O 작업중 논리적 실패 발생
    * badbit <- 읽기 쓰기 오류로 인한 실패
    */
	shaderFile.exceptions(
		std::ifstream::failbit | std::ifstream::badbit
	);
    try {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        // 파일 내용을 스트림에 읽어옵니다.
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        // 스트림을 std::string으로 변환합니다.
        return shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        return "";
    }
}

GLuint Shader::compileShader(const char* shaderCode, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);

    // 컴파일 오류 확인
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint Shader::getVertexShader(const char* filePath) {
    auto vertexShader = shaderMap.find(filePath);

    if (vertexShader == shaderMap.end()) {
        std::string vertexShaderSource = readShaderSource(filePath);
        std::string processedVertexShaderSource = ProcessShaderIncludes(vertexShaderSource, "./shader");

        GLuint vertexShaderID = compileShader(processedVertexShaderSource.c_str(), GL_VERTEX_SHADER);
        shaderMap[filePath] = vertexShaderID;
        return vertexShaderID;
    }

    return vertexShader->second;
}

GLuint Shader::getFragmentShader(const char* filePath) {
    auto fragmentShader = shaderMap.find(filePath);

	if (fragmentShader == shaderMap.end()) {
		std::string fragmentShaderSource = readShaderSource(filePath);
        std::string processedFragmentShaderSource = ProcessShaderIncludes(fragmentShaderSource, "./shader");

		GLuint fragmentShaderID = compileShader(processedFragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);
		shaderMap[filePath] = fragmentShaderID;
		return fragmentShaderID;
	}

	return fragmentShader->second;
}

unsigned int Shader::loadShaderProgram(
    const char* vertexShaderPath,
    const char* fragmentShaderPath,
    const char* shaderProgramName
) {
    auto shaderProgramId = shaderProgramMap.find(shaderProgramName);
    auto vertexShaderId = getVertexShader(vertexShaderPath);
    auto fragmentShaderId = getFragmentShader(fragmentShaderPath);

    if(shaderProgramId == shaderProgramMap.end()) {
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShaderId);
		glAttachShader(shaderProgram, fragmentShaderId);
		glLinkProgram(shaderProgram);

		// 링크 오류 확인
		GLint success;
		GLchar infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		shaderProgramMap[shaderProgramName] = shaderProgram;

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        shaderMap.erase(vertexShaderPath);
        shaderMap.erase(fragmentShaderPath);

		return shaderProgram;
	}

    return shaderProgramId->second;
}

unsigned int Shader::getShaderProgram(const char* shaderProgramName) {
	auto shaderProgramId = shaderProgramMap.find(shaderProgramName);

	if (shaderProgramId == shaderProgramMap.end()) {
		std::cerr << "ERROR::SHADER::PROGRAM::NOT_FOUND" << std::endl;
		return 0;
	}

	return shaderProgramId->second;
}

std::string Shader::ProcessShaderIncludes(const std::string& source, const std::string& directoryPath) {
    std::istringstream stream(source);
    std::string line;
    std::string processedSource;
    while (std::getline(stream, line)) {
        if (line.substr(0, 8) == "#include") {
            std::string includeFileName = line.substr(9).erase(0, line.find_first_not_of(" \t")); // 파일명 추출 및 앞쪽 공백 제거
            std::string includeFilePath = directoryPath + "/" + includeFileName;
            std::string includeContent = readShaderSource(includeFilePath.c_str());
            if (!includeContent.empty()) {
                processedSource += includeContent + "\n";
            }
        }
        else {
            processedSource += line + "\n";
        }
    }
    return processedSource;
}

void Shader::setBool(const char* shaderProgramName, const char* name, bool value) {
	glUniform1i(glGetUniformLocation(getShaderProgram(shaderProgramName), name), (int)value);
}

void Shader::setInt(const char* shaderProgramName, const char* name, int value) {
	glUniform1i(glGetUniformLocation(getShaderProgram(shaderProgramName), name), value);
}

void Shader::setFloat(const char* shaderProgramName, const char* name, float value) {
	glUniform1f(glGetUniformLocation(getShaderProgram(shaderProgramName), name), value);
}

void Shader::setVec2(const char* shaderProgramName, const char* name, const glm::vec2& value) {
	glUniform2fv(glGetUniformLocation(getShaderProgram(shaderProgramName), name), 1, &value[0]);
}

void Shader::setVec2(const char* shaderProgramName, const char* name, float x, float y) {
	glUniform2f(glGetUniformLocation(getShaderProgram(shaderProgramName), name), x, y);
}

void Shader::setVec3(const char* shaderProgramName, const char* name, const glm::vec3& value) {
	glUniform3fv(glGetUniformLocation(getShaderProgram(shaderProgramName), name), 1, &value[0]);
}

void Shader::setVec3(const char* shaderProgramName, const char* name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(getShaderProgram(shaderProgramName), name), x, y, z);
}

void Shader::setVec4(const char* shaderProgramName, const char* name, const glm::vec4& value) {
	glUniform4fv(glGetUniformLocation(getShaderProgram(shaderProgramName), name), 1, &value[0]);
}

void Shader::setVec4(const char* shaderProgramName, const char* name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(getShaderProgram(shaderProgramName), name), x, y, z, w);
}

void Shader::setMat2(const char* shaderProgramName, const char* name, const glm::mat2& mat) {
	glUniformMatrix2fv(glGetUniformLocation(getShaderProgram(shaderProgramName), name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const char* shaderProgramName, const char* name, const glm::mat3& mat) {
	glUniformMatrix3fv(glGetUniformLocation(getShaderProgram(shaderProgramName), name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const char* shaderProgramName, const char* name, const glm::mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(shaderProgramName), name), 1, GL_FALSE, &mat[0][0]);
}


