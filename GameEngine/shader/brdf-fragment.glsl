#version 460 core
#include Importance-sample.glsl

out vec2 FragColor;
in vec2 TexCoords;

void main(){
	float NdotV = TexCoords.x;
	float roughness = TexCoords.y;

	vec3 V;
	V.x = sqrt(1.0 - NdotV*NdotV);
	V.y = 0.0;
	V.z = NdotV;

	float A = 0.0;
	float B = 0.0;

	vec3 N = vec3(0.0,0.0,1.0);

	const uint SAMPLE_COUNT = 1024;

	for(int i = 0; i < SAMPLE_COUNT; i++){
		vec2 XI = Hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(XI, N, roughness);
		vec3 L = normalize(2.0 * dot(V,H) * H - V);

		float NdotL = max(L.z, 0.0);
		float NdotH = max(H.z, 0.0);
		float VdotH = max(dot(V,H), 0.0);

		if(NdotL > 0.0){
			float G = GeometrySmith(N, V, L, roughness);
			float G_Vis = (G * VdotH) / (NdotH * NdotV);
			float Fc = pow(1.0 - VdotH, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	A /= float(SAMPLE_COUNT);
	B /= float(SAMPLE_COUNT);

	FragColor = vec2(A,B);
}