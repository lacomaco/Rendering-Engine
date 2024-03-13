1. Deferred Shading (1일)
https://learnopengl.com/Advanced-Lighting/Deferred-Shading

2. FXAA (2일) (Deferred Shading으로 아키텍처를 바꾸면 MSAA 사용이 불가능해짐. 그래서 FXAA로 안티얼라이징해야함.)
https://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html

3. SSAO (2일)
https://learnopengl.com/Advanced-Lighting/SSAO

4. Lens Flare (1일)
https://www.youtube.com/watch?v=OiMRdkhvwqg

5. depth of field (될려나?)
 https://developer.nvidia.com/gpugems/gpugems/part-iv-image-processing/chapter-23-depth-field-survey-techniques

6. HDRI with auto exposure (미정)
https://bruop.github.io/exposure/
-----------------------------
Deferred Shading에 대해서.

G-Buffer 구성도

0번 텍스처 
	rgb: position
    a: metallic (0~1)
1번 텍스처
    rgb: albedo
    a: roughness
2번 텍스처
    rgb: normal
    a: 할당되지 않음 0.0
3번 텍스처
    rgb: emissive
    a: 할당되지 않음 0.0
4번 텍스처
    rgba: 갓레이를 위해서 사용
5번 텍스처
    rgba: SSAO를 위해서 사용

