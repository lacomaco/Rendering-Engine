#version 460 core
#include common.glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangentModel;
layout (location = 4) in vec3 aBitangent;

out vec3 tangent;
out vec3 normal;
out vec3 biTangent;
out mat3 TBN;
out vec2 TexCoords;
out vec3 FragPos;

void main()
{
	tangent = aTangentModel;
	normal = aNormal;
	biTangent = aBitangent;

	normal = normalize(invTranspose * aNormal);
	tangent = normalize(invTranspose * aTangentModel);

	if (dot(cross(normal, tangent), biTangent) < 0.0){
	    tangent = tangent * -1.0;
	}

	tangent = normalize(tangent - dot(tangent, normal) * normal);
	biTangent = normalize(cross(normal, tangent));

	 TBN = mat3(tangent,biTangent,normal);

    gl_Position = projection * view * model* vec4(aPos, 1.0);

	TexCoords = aTexCoord;

	FragPos = vec3(model * vec4(aPos, 1.0));
}  