uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 invTranspose;


uniform sampler2D albedo0;
uniform sampler2D specular0;

struct Material {
	vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

struct Light {
	vec3 strength;
	float fallOffStart;
	vec3 direction;
    float fallOffEnd;
    vec3 position;
    float spotPower;
    int lightType;

    // phong shading 전용 PBR에서 지울거임.
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

uniform vec3 cameraPos;
