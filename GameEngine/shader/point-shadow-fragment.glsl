#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform vec3 far_plane;

void main(){
  float lightDistance = length(FragPos.xyz - lightPos);
  lightDistance = lightDistance / far_plane;

  gl_FragDepth = lightDistance;
}
