#version 430 core

#define ThreadGroupX 16
#define ThreadGroupY 16

#include SH-common.glsl

uniform samplerCube skyMap;

layout(std430, binding = 0) buffer DataBuffer {
	float data[];
};

layout(local_size_x = ThreadGroupX, local_size_y = ThreadGroupY, local_size_z = 1) in;

shared vec3 SharedCoeffs[ThreadGroupX * ThreadGroupY][9];
shared int TotalSample;

vec3 irradianceLight(vec3 normal)	{
	float sampleDelta = 0.025;
	float nrSamples = 0.0;

	vec3 up = vec3(0.0,1.0,0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal,right));

	vec3 irradiance = vec3(0.0f);

	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){
			vec3 tangentSample = vec3(sin(theta)*cos(phi), sin(theta) * sin(phi),cos(theta));

			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(skyMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	return 2.0 * PI * irradiance * (1.0 / float(nrSamples)); 
}

void main () {
	if(gl_LocalInvocationIndex == 0){
		// 0번 스레드만 초기화 해야함!
		TotalSample = 0;
	}

	vec3 SHCoeffs[9];
	for(int i = 0; i < 9; i++) {
		SHCoeffs[i] = vec3(0.0);
	}


	int nrSamples = 0;
	const float SampleDelta = 0.025;

	const float DeltaPhi = 2.0 * PI / float(ThreadGroupX); 
	const float DeltaTheta = PI / float(ThreadGroupY);

	float startPhi = gl_LocalInvocationID.x * DeltaPhi;
	float endPhi = startPhi + DeltaPhi;

	float startTheta = gl_LocalInvocationID.y * DeltaTheta;
	float endTheta = startTheta + DeltaTheta;

	// 지금은 normal (0,1,0) 방향으로만 샘플링하고있어서 ziggle현상이바 ㄹ생함

	// 4중 for문을 돌아야함

	for(float phi = startPhi; phi < endPhi; phi += SampleDelta){
		for(float theta = startTheta; theta < endTheta; theta += SampleDelta){
		    vec3 normal = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 color = irradianceLight(normal);

			float y[9];
			L2(theta,phi, y);

			for(int i = 0; i < 9; i++){
				SHCoeffs[i] += color * y[i] * sin(theta);
			}

			nrSamples++;
		}
	}

	int sharedIndex = int(gl_GlobalInvocationID.y) * ThreadGroupX + int(gl_GlobalInvocationID.x);

	for(int i = 0; i < 9; i++){
		SharedCoeffs[sharedIndex][i] = SHCoeffs[i];
	}

	atomicAdd(TotalSample, nrSamples);
	groupMemoryBarrier();
	barrier();

	if(gl_LocalInvocationIndex == 0){
		for(int i = 0; i < ThreadGroupX * ThreadGroupY; i++){
			for(int j = 0; j < 9; j++){
				data[j * 3] += SharedCoeffs[i][j].r;
				data[j * 3 + 1] += SharedCoeffs[i][j].g;
				data[j * 3 + 2] += SharedCoeffs[i][j].b;
			}
		}

		float normalizationFactor = 4.0 * PI / float(TotalSample);
		// Monte Carlo
		for(int j = 0; j < 9; j++){
			data[j * 3] *= normalizationFactor;
			data[j * 3 + 1] *= normalizationFactor;
			data[j * 3 + 2] *= normalizationFactor;
		}
	}
}