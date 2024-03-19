#version 460 core

uniform sampler2D sceneTexture;

out vec4 FragColor;
in vec2 TexCoords;

void main() {
    FragColor = texture(sceneTexture, TexCoords);
}