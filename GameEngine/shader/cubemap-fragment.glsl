#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyBox;
uniform samplerCube radianceMap;
uniform samplerCube irradianceMap;

void main()
{    
    FragColor = texture(skyBox, TexCoords);
}