#include "./MeshMaterialsLight.h"
#include "../Constants.h"
#include <glew.h>

using namespace std;

std::vector<Texture> textures_loaded;

Mesh::Mesh(std::vector<Vertex> _vertices,
	std::vector<unsigned int> _indices,
	std::vector<Texture>_textures) :
	vertices(_vertices),
	indices(_indices),
	textures(_textures)
{
	CalculateVertexAveragePosition();

	for (auto& texture : _textures) {
		if (texture.type == "albedo" && texture.isAlpha) {
			isAlphaMesh = true;
			break;
		}
	}

	setupMesh();
}

void Mesh::CalculateVertexAveragePosition(glm::vec3 scale) {
	glm::vec3 center = glm::vec3(0.0f);

	for (auto& vertex : vertices) {
		center += vertex.position * scale;
	}

	center /= vertices.size();

	position = center;
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::Draw(const char* shaderProgramName,int instanceCount) {
	auto shader = Shader::getInstance();
	mat.PutMaterialUniforms(shaderProgramName);
	auto program = shader->getShaderProgram(shaderProgramName);

	glUseProgram(program);
	
	// Phong
	unsigned int specularNr = 0;

	// PBR
	unsigned int albedoNr = 0;
	unsigned int emissiveNr = 0;
	unsigned int normalNr = 0;
	unsigned int heightNr = 0;
	unsigned int metallicNr = 0;
	unsigned int aoNr = 0;
	unsigned int roughnessNr = 0;

	// turoOff all Type Texture
	shader->setInt(shaderProgramName, "use_albedo0", 0);
	shader->setInt(shaderProgramName, "use_emissive0", 0);
	shader->setInt(shaderProgramName, "use_normal0", 0);
	shader->setInt(shaderProgramName, "use_height0", 0);
	shader->setInt(shaderProgramName, "use_metallic0", 0);
	shader->setInt(shaderProgramName, "use_ao0", 0);
	shader->setInt(shaderProgramName, "use_roughness0", 0);


	// 0,1,2 텍스처는 큐브매핑의 스카이박스, 라디언스, 이리디언스 맵이 예약중.
	for (unsigned int i = 0; i < textures.size(); i++) {
		int textureNumber = i + TEXTURE_START;

		glActiveTexture(GL_TEXTURE0 + textureNumber);

		std::string number;
		auto name = textures[i].type;

		if (name == "specular") {
			number = std::to_string(specularNr++);
		} 

		if (name == "albedo") {
			number = std::to_string(albedoNr++);
		}

		if (name == "emissive") {
			number = std::to_string(emissiveNr++);
		}

		if (name == "normal") {
			number = std::to_string(normalNr++);
		}

		if (name == "height") {
			number = std::to_string(heightNr++);
		}

		if (name == "metallic") {
			number = std::to_string(metallicNr++);
		}

		if (name == "ao") {
			number = std::to_string(aoNr++);
		}

		if (name == "roughness") {
			number = std::to_string(roughnessNr++);
		}
		
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		shader->setInt(shaderProgramName, (name + number).c_str(), textureNumber);
		shader->setBool(shaderProgramName, ("use_" + name + number).c_str(), 1);
	}

	// draw mesh
	glBindVertexArray(VAO);
	if (instanceCount == 0) {
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
	}

	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i + TEXTURE_START);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW
	);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);

	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));

	// texcoord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));

	// tangentModel
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));

	// color
	// color는 추후 제거될수도있음 일단은 추가하였음.
	// 제거시 Vertex color 제거 필요.
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int),
		indices.data(),
		GL_STATIC_DRAW
	);
}

void Mesh::CalculateTangents() {
	for (size_t i = 0; i < indices.size(); i += 3) {
		Vertex& v0 = vertices[indices[i]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		// Tangent 구하는 공식.
		// see here : https://learnopengl.com/Advanced-Lighting/Normal-Mapping
		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		float deltaU1 = v1.texcoord.x - v0.texcoord.x;
		float deltaV1 = v1.texcoord.y - v0.texcoord.y;
		float deltaU2 = v2.texcoord.x - v0.texcoord.x;
		float deltaV2 = v2.texcoord.y - v0.texcoord.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		glm::vec3 tangent;
		tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

		// vertex가 공유되는 경우 평균으로 처리한다.
		v0.tangentModel += tangent;
		v1.tangentModel += tangent;
		v2.tangentModel += tangent;
	}

	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].tangentModel = glm::normalize(vertices[i].tangentModel);
	}
}


void Material::PutMaterialUniforms(const char* shaderProgramName) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(shaderProgramName);

	auto fresnelData = FresnelData[type];

	glUseProgram(program);

	shader->setVec3(shaderProgramName, "material.ambient", ambient);
	shader->setVec3(shaderProgramName, "material.specular", specular);
	shader->setVec3(shaderProgramName, "material.diffuse", diffuse);
	shader->setFloat(shaderProgramName, "material.shininess", shininess);
	shader->setVec3(shaderProgramName, "material.fresnelIRO", fresnelData);
}
