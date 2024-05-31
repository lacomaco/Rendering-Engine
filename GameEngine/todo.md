1. 구현해야할것

[] CSM
[] PCSS
[] Fog Light
[] VPL
[] Frustum Culling
[] Materials xml에 정보 저장하여 사용하도록 수정
[] xml에 씬정보 저장하여 불러올 수 있도록 시스템 수정
[] Killer Feature PCSS 그림자
[] Light Baking (Irradiance? 방법 찾아보기)
[] Volumatric Light 추가 (Screen-Space God rays는 제거)
[] Volumatric Cloud
[] Animation
[] 자연 애니메이션
[] GI

2. 고려할것

포스트 프로세싱 태우는 것들을 4각형 vertex로 처리하고 있는데 compute shader에서 처리해도 괜찮을듯 싶다. 시간 여유가 되면 바꿔보자.

Deferred shading으로 변경해도 괜찮을듯 싶다. (G Buffer는 이미 구현되어져있다.)
Deferred shading으로 변경시 alpha값은 포기하고 멀티샘플링은 직접 알고리즘으로 구현해야한다. 시간이 많이 들것이다.

------ 현재 진행하는 리스트 ------

Cascade Shadow Map 진행중
Light - Shadow 대규모 리팩토링중

목표

Light 정보 UBO로 전환
Shadow Atlas를 통해서 쉐도우맵 전달
Directional Light, Spot Light, Point Light 그림자 체크
