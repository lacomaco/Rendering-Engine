#version 460 core

out vec4 FragColor;
uniform bool isGodRay;

void main() {
    if (isGodRay) {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	} else {
		FragColor = vec4(0.0,0.0,0.0,1.0);
	}
}
