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
* 
* 3: directional light 그림자맵 예약
* 4: point light 그림자 큐브맵 예약.
* 5: point light 그림자 큐브맵 예약.
* 6: spot light 그림자맵 예약.
* 7: spot light 그림자맵 예약.
* 
* 나중에 그림자맵 아틀라스로 변경할수도 있음.
* 
* 가장 가까운 
*/
#ifndef TEXTURE_SKYBOX_OFFSET
#define TEXTURE_SKYBOX_OFFSET 3
#endif

#ifndef TEXTURE_START
#define TEXTURE_START 8
#endif

#ifndef BIAS
#define BIAS 0.005f
#endif