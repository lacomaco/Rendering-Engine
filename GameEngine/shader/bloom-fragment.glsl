#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform bool isHorizontal;

float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main(){
    // 2번째 인자는 LOD임
    vec2 textureOffset = 1.0 / textureSize(screenTexture, 0);
    vec3 result = texture(screenTexture, TexCoords).rgb * weight[0];

    if(isHorizontal){
        for(int i = 1; i < 5; i++){
           result += texture(screenTexture, TexCoords + vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
           result += texture(screenTexture, TexCoords - vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
       for(int i = 1; i < 5; i++){
           result += texture(screenTexture, TexCoords + vec2(0.0, textureOffset.y * i)).rgb * weight[i];
		   result += texture(screenTexture, TexCoords - vec2(0.0, textureOffset.y * i)).rgb * weight[i];
       }
    }

    FragColor = vec4(result, 1.0);
}
