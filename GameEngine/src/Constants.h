#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 1280
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 960
#endif
/*
* 0: ť����� ��ī�̹ڽ� ����
* 1: ť����� ���� �� ����
* 2: ť����� �̸���� �� ����.
* 3: brdf��� �ؽ�ó
* 
* 4: directional light �׸��ڸ� ����
* 5: point light �׸��� ť��� ����.
* 6: point light �׸��� ť��� ����.
* 7: spot light �׸��ڸ� ����.
* 8: spot light �׸��ڸ� ����.
* 
* ���߿� �׸��ڸ� ��Ʋ�󽺷� �����Ҽ��� ����.
* 
* ���� ����� 
*/
#ifndef TEXTURE_SKYBOX_OFFSET
#define TEXTURE_SKYBOX_OFFSET 4
#endif


/*
* 9: albedo
* 10: metallic
* 11: roughness
* 12: normal
* 13: ao
* 14: emissive
*/
#ifndef TEXTURE_START
#define TEXTURE_START 9
#endif

// ����!!! OPENGL ES <- �ؽ�ó 8���ۿ� ��� ����, OpenGL 3.x �̻� �������� �ּ� 16���� ������ �ؽ�ó�� �����Ѵٰ���.
// Opengl 4 ���� �̻��� ���� �������� ũ�� �����ɰ� ������, Ȥ�� �𸣴� ����ص���!
// �ؽ�ó �ø��°� ��������!

#ifndef BIAS
#define BIAS 0.005f
#endif


// UBO ���
#ifndef CAMERA_UBO
#define CAMERA_UBO 0
#endif

#ifndef LIGHT_UBO
#define LIGHT_UBO 1
#endif

#ifndef SHADOW_UBO
#define SHADOW_UBO 2
#endif

#ifndef SHADOW_RESOLUTION
#define SHADOW_RESOLUTION 1024
#endif

// 9 * 9 �׸��ڸ��� ��.
#ifndef SHADOW_ATLAS
#define SHADOW_ATLAS 9216
#endif