const float PI = 3.141592;

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi,vec3 N,float roughness){
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    /*
    아래 수식은 G항을 PDF -> CDF로 유도하여 만든 수식이다.
    https://agraphicsguynotes.com/posts/sample_microfacet_brdf/
    Xi.y는 CDF 값으로 0 ~ 1 사이의 값을 가지고 있다.
    아래 수식을 사용하여 cos값을 구할 수 있다.
    */
    float cosTheta = sqrt((1.0 - Xi.y)/(1.0 + (a*a - 1.0)*Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    vec3 cartesianCoordinate;
    cartesianCoordinate.x = cos(phi) * sinTheta;
    cartesianCoordinate.y = sin(phi) * sinTheta;
    cartesianCoordinate.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0,0.0,1.0) : vec3(1.0,0.0,0.0);
    vec3 tangent = normalize(cross(up,N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * cartesianCoordinate.x + 
    bitangent * cartesianCoordinate.y + 
    N * cartesianCoordinate.z;

    return normalize(sampleVec);
}