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
* 0: 큐브매핑 스카이박스 예약
* 1: 큐브매핑 라디언스 맵 예약
* 2: 큐브매핑 이리디언스 맵 예약.
* 3: brdf룩업 텍스처
* 
* 4: directional light 그림자맵 예약
* 5: point light 그림자 큐브맵 예약.
* 6: point light 그림자 큐브맵 예약.
* 7: spot light 그림자맵 예약.
* 8: spot light 그림자맵 예약.
* 
* 나중에 그림자맵 아틀라스로 변경할수도 있음.
* 
* 가장 가까운 
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

// 주의!!! OPENGL ES <- 텍스처 8개밖에 허용 안함, OpenGL 3.x 이상 버전에선 최소 16개의 유니폼 텍스처를 지원한다고함.
// Opengl 4 버전 이상을 쓰고 있음으로 크게 문제될건 없지만, 혹시 모르니 기억해두자!
// 텍스처 늘리는건 조심하자!

#ifndef BIAS
#define BIAS 0.005f
#endif


// UBO 목록
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

// 9 * 9 그림자맵이 들어감.
#ifndef SHADOW_ATLAS
#define SHADOW_ATLAS 9216
#endif