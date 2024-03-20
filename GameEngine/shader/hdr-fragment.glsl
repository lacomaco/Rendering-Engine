#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform sampler2D godRayTexture;
uniform sampler2D ssaoTexture;
uniform sampler2D lensFlareTexture;

uniform bool useSSAO;

uniform float exposure;
uniform float bloomThreshold;

const float gamma = 2.2;

vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

    color = mix(color,bloomColor, bloomThreshold);
    color += texture(lensFlareTexture, TexCoords).rgb;
    
    if(useSSAO){
        color *= texture(ssaoTexture, TexCoords).r;
    }

    vec3 mapped = aces(color * log(exp2(exposure)));
    mapped = pow(mapped, vec3(1.0 / gamma));

    mapped += texture(godRayTexture, TexCoords).rgb;

    FragColor = vec4(mapped, 1.0);
}