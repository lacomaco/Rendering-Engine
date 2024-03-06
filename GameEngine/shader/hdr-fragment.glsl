#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float exposure;

void main()
{ 
    vec3 color = texture(screenTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

    color += bloomColor;

    // Reinhard tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * exposure);


    /*
    glEnable(GL_FRAMEBUFFER_SRGB);를 사용하고 있어서 감마값 처리를 하고있지 않음.
    만약 수동으로 처리하고싶다면

    위에

    const flaot gamma = 2.2;

    ...

    mapped = pow(mapped, vec3(1.0 / gamma));

    수동 감마로 1/2.2 처리를 해줘야함.
    */
    FragColor = vec4(mapped, 1.0);
}