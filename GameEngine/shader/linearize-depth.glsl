uniform float near = 0.1;
uniform float far = 100.0;

/*
see 1. http://web.archive.org/web/20130426093607/http://www.songho.ca/opengl/gl_projectionmatrix.html
see 2. http://web.archive.org/web/20130416194336/http://olivers.posterous.com/linear-depth-in-glsl-for-real

알아야 하는 전제 지식.

1. A,B는 perspective Matrix의 3,4번째 열에 있는 값이다.

[
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, A, B,
	0, 0, -1, 0
]; (실제로 0,과 1은 아니다. see1. 성호님의 블로그에서 3rd row of GL_PROJECTION matrix like this. 아래 있는 매트릭스를 확인하라.)

2. OpenGL에서는 z값을 perspectiveMatrix로 통과 시킨 후 w 컴포넌트로 나눈다. ( w = -z )

이를 표현한 공식이 zn = ze / we
(여기서 zn은 NDC에 있는 z, ze는 eye space에 있는 w이다.)

행렬에서 공식을 축출하면 zn = (A * ze + B*we) / - ze이다.

A의 경우 - (f+n) / (f-n) 이고, B의 경우 - (2fn) / (f-n) 이다.

------------------------------------------------------------------------------------------------------
1. 먼저 z값을 NDC로 투영해야한다. (see below) 투영하면 그 값이 zn이다.
2. ze를 구하는 공식을 역순으로 전개하자.. (see below)

ze = (B*we)/(zn - A) ( 여기서 eye space에서 w컴포넌트 값은 1이기에 줄일 수 있음)
ze = B / (zn-A)

공식을 전개하면

(2.0 * near * far) / (fajk
r + near - z * (far - near)); 이러한값이 나온다.


*/
float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; // Back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));	
}
