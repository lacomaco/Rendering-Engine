#include "Game.h"
#include <SDL.h>
#include <GL/glew.h>
#include "../Util/Shader.h"
#include "../Util/stb_image.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include "./ImguiController.h"

Game::Game() {
	mWindow = nullptr;
	mIsRunning = true;

}

bool Game::Initialize() {
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0) {
		SDL_Log("SDL 초기화 실패: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"My Game Practices",
		100,
		100,
		1024,
		768,
		SDL_WINDOW_OPENGL
	);

	if (!mWindow) {
		SDL_Log("윈도우 생성 실패 %s", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

	context = SDL_GL_CreateContext(mWindow);

	if (!context) {
		SDL_Log("GL Context 생성 실패! %s", SDL_GetError());
	}

	if (glewInit() != GLEW_OK) {
		SDL_Log("GLEW 초기화 실패!");
		return false;
	}

	glViewport(0, 0, 1024, 768);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	SetOpenGL();

	ImguiController::CreateInstance(mWindow, context);
	imguiController = ImguiController::getInstance();

	return true;
}

void Game::Shutdown() {
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	SDL_GL_DeleteContext(context);
	delete imguiController;
}

void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type) {
			//  윈도우의 x 버튼을 누르면 게임 종료
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// 키보드 키가 눌렸는지 감지
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}
}

void Game::UpdateGame() {

	/*
	* fps를 60으로 제한한다. (60FPS면 얼추 16.6ms이다.)
	* 
	* PS: fps를 60으로 제한하는 이유
	* 
	* 물리 계산을 매번 Update할때 프레임이 달라지면 물리 게산 결과도 달라진다.
	* 여기선 가장 쉬운 방법으로 최대 fps를 60으로 제한 거는 방법으로 해결함
	*/
	while (!SDL_TICKS_PASSED(SDL_GetTicks(),mTicksCount + 16));

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	mTicksCount = SDL_GetTicks();

	// 델타 시간이 너무 크면 게임이 멈추는 것을 방지
	/*
	* 이 로직이 필요한 이유.
	* 만약 sleep을 걸었거나, 디버거로 시스템을 잠구고 다시 실행시키면
	* deltaTime이 크게 불어나는 경우가 발생한다.
	* 이를 방지하기 위해서 deltaTime이 0.05f를 넘지 않도록 제한한다.
	* 60fps로 제한을 걸어두었기 때문에 일반적인 상황에선 deltaTime은
	* 0.0166f 정도가된다.
	*/
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}
}

void Game::GenerateOutput() {
	imguiController->Update();

	float timeValue = SDL_GetTicks() / 1000.0f;
	float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

	auto program = Shader::getInstance()->getShaderProgram("triangle");
	// 만약 찾지 못하면 -1이다.
	int vertexColorLocation =
		glGetUniformLocation(program,"ourColor");
	glUseProgram(program);
	glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
	glBindVertexArray(VAO);
	// 텍스처 바인딩

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(program, "texture1"), 0);
	glUniform1i(glGetUniformLocation(program, "texture2"), 1);


	// 하얀색으로 초기화.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	* 6: 인덱스 갯수
	* GL_UNSIGNED_INT: 인덱스 타입
	* 0: 인덱스 배열 시작 위치
	*/
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	imguiController->Render();
	SDL_GL_SwapWindow(mWindow);
}

void Game::SetOpenGL() {
	// 텍스처 로드
	int width, height, nrChannels;
	unsigned char* data = stbi_load("./assets/images/wall.jpg", &width, &height, &nrChannels, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	/*
	* S: 수평축 (x축)
	* T: 수직축 (y축)
	* 특별한 약어가 있는것은 아니다. 그냥 전통적인 명명법이라한다.
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/*
	* 
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, // 텍스처 바인딩 대상 지정, 큐브맵은 GL_TEXTURE_3D이다.
		0, // mipmap 레벨
		GL_RGB, // internal formap
		width, 
		height,
		0, // border 크기
		GL_RGB, // 입력 데이터 형식
		GL_UNSIGNED_BYTE, // 픽셀 데이터 타입
		data // 픽셀 데이터 포인터.
		);
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	unsigned char* data2 = stbi_load("./assets/images/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	/*
	* mipmap? : 텍스처 매핑 기법.
	* GPU에서 텍스처를 미리 절반으로 줄인 작은 사이즈의 텍스처를 만들어둔다.
	* ex) 256* 256 -> 128 * 128 -> 64 * 64 -> 32 * 32 -> 16 * 16 -> 8 * 8 -> 4 * 4 -> 2 * 2 -> 1 * 1
	* mipmap level 역시 이에 매칭된다.
	* mipmap 0 -> 256 * 256
	* mipmap 1 -> 128 * 128
	* 
	* 만약 거리가 먼 물체를 렌더링할때 굳이 해상도가 높은 텍스처를 쓸 필요가 없으니
	mipmap을 적절히 조절하면 최적화와 시각적 품질을 얻을 수 있다.
	*/
	stbi_image_free(data);
	stbi_image_free(data2);


	// 셰이더 로드
	auto shader = Shader::getInstance();
	auto program = shader->loadShaderProgram(
		"./shader/triangle-vertex.glsl",
		"./shader/triangle-fragment.glsl",
		"triangle"
	);

	// VAO, VBO, EBO 생성
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/*
	* 이 코드의 의미.
	* 
	* GL_ARRAY_BUFFER에 바인딩된 Buffer OBJECT에
	* vertices.size() * sizeof(float) 만큼의 메모리를 할당하고
	* vertices.data()의 데이터를 복사한다.
	* 또한 GL_STATIC_DRAW로 설정하여 데이터가 변하지 않음을 명시한다.
	*/
	glBufferData(
		GL_ARRAY_BUFFER, 
		vertices.size() * sizeof(float),
		vertices.data(), 
		GL_STATIC_DRAW
	);

	// position
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	/*
	* color
	* 1: attribute location
	* 3: size
	* GL_FLOAT: type
	* GL_FALSE: normalized
	* 3 * sizeof(float): stride
	* (void*)(3 * sizeof(float)): offset
	*
	*/

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int),
		indices.data(),
		GL_STATIC_DRAW
	);
}

void Game::AddActor(Actor* actor)
{
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}

}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}
