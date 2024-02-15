#include "./VertexType.h"
#include <GL/glew.h>

using namespace std;

/*
Mesh::Mesh(std::vector<Vertex>&& vertices,
	std::vector<unsigned int>&& indices) : 
	vertices(std::move(vertices)),
	indices(std::move(indices)) 
{
	setupMesh();
}
*/

Mesh::Mesh(std::vector<Vertex>&& _vertices,
	std::vector<unsigned int>&& _indices,
	std::vector<Texture>&& _textures) :
	vertices(std::move(_vertices)),
	indices(std::move(_indices)),
	textures(std::move(_textures))
{
	setupMesh();
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::Draw(const char* shaderProgramName) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(shaderProgramName);

	glUseProgram(program);

	unsigned int albedoNr = 0;
	unsigned int emissiveNr = 0;
	unsigned int normalNr = 0;
	unsigned int heightNr = 0;
	unsigned int metallicNr = 0;
	unsigned int aoNr = 0;
	unsigned int roughnessNr = 0;
	unsigned int specularNr = 0;


	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = textures[i].type;

		if (name == "albedo") {
			number = std::to_string(albedoNr++);
		}
		else if ("emissive") {
			number = std::to_string(emissiveNr++);
		}
		else if (name == "normal") {
			number = std::to_string(normalNr++);
		}
		else if (name == "height") {
			number = std::to_string(heightNr++);
		}
		else if (name == "metallic") {
			number = std::to_string(metallicNr++);
		}
		else if (name == "ao") {
			number = std::to_string(aoNr++);
		}
		else if (name == "roughness") {
			number = std::to_string(roughnessNr++);
		}
		else if (name == "specular") {
			number = std::to_string(specularNr++);
		}


		shader->setInt(shaderProgramName, (name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	

	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW
	);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texcoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// tangentModel
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// color
	// color는 추후 제거될수도있음 일단은 추가하였음.
	// 제거시 Vertex color 제거 필요.
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
