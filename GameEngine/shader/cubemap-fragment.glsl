#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyBox;
uniform samplerCube preFilterEnvironmentMap;
uniform samplerCube irradianceMap;

uniform int select;
uniform float lodLevel;

void main()
{     
    if(select == 0){
        FragColor = textureLod(skyBox, TexCoords, lodLevel);
    } else if(select == 1){
        FragColor = textureLod(preFilterEnvironmentMap, TexCoords,lodLevel);
	} else {
        FragColor = textureLod(irradianceMap, TexCoords,lodLevel);
    }
}