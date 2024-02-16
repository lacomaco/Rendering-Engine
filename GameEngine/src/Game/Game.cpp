#include "Game.h"
#include <SDL.h>
#include <glew.h>
#include "../Util/Shader.h"
#include "../Util/stb_image.h"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "./ImguiController.h"
#include "../GameObject/PrimitiveObject/Plane.h"
#include "../Constants.h"

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
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
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

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	ImguiController::CreateInstance(mWindow, context);
	imguiController = ImguiController::getInstance();

	stbi_set_flip_vertically_on_load(true);

	// 쉐이더 생성

	auto shader = Shader::getInstance();
	shader->loadShaderProgram(
		"./shader/default-vertex.glsl",
		"./shader/default-fragment.glsl",
		"default"
	);

	shader->loadShaderProgram(
		"./shader/normal-vertex.glsl",
		"./shader/normal-fragment.glsl",
		"normal",
		"./shader/normal-geometry.glsl"
	);

	// 화면에 그릴 오브젝트들 생성
	//plane = new Plane();
	//box = new Box();
	circle = new Circle();

	//backPack = new Model("./assets/zeldaPosed001/zeldaPosed001.fbx");
	 //backPack = new Model("./assets/pbrSponza/sponza/Sponza.gltf");

	light = new Light();
	light->box->scale = glm::vec3(0.05f, 0.05f, 0.05f);



	camera = new Camera(
		45.0f,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);

	camera->cameraPos = glm::vec3(0.0f, 0.6f, 3.0f);

	// 거리좀 떨어져서 보이게 하기 위해서 x축 0.5씩 이동
	{
		//plane->position = glm::vec3(-0.5f, 0.0f, 0.0f);
		//box->position = glm::vec3(0.0f, 0.0f, 0.0f);
		//circle->position = glm::vec3(0.5f, 0.0f, 0.0f);
	}

	// 스케일이 조정

	{
		//plane->scale = glm::vec3(0.2f, 0.2f, 0.2f);
		//box->scale = glm::vec3(0.2f, 0.2f, 0.2f); 
		// backPack->scale = glm::vec3(0.01f, 0.01f, 0.01f);
	}

	input = Input::GetInstance();

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
		case SDL_QUIT:
			mIsRunning = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_RIGHT) {
				input->rightMouseButtonDown = true;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			input->rightMouseButtonDown = false;
			break;
		}
	}

	// 키보드 키가 눌렸는지 감지
	const Uint8* state = input->state;
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	if (state[SDL_SCANCODE_F]) {
		camera->ResetPitch();
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

	camera->Update(deltaTime);

	input->SetMouse();


	// 3개 업데이트.
	{
		//plane->rotation += glm::vec3(0.0f,90.0f,30.0f) * deltaTime;
		//box->rotation += glm::vec3(3.0f, 3.0f, 3.0f) * deltaTime;
		//circle->rotation += glm::vec3(3.0f, 3.0f, 3.0f) * deltaTime;
	}
}

void Game::GenerateOutput() {
	imguiController->Update();

	float timeValue = SDL_GetTicks() / 1000.0f;

	auto program = Shader::getInstance()->getShaderProgram("default");
	// 만약 찾지 못하면 -1이다.
	glUseProgram(program);

	// 하얀색으로 초기화.
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	camera->putCameraUniform("default");

	//backPack->Draw("default");
	circle->Draw("default");
	light->Draw("default");

	auto showNormal = imguiController->showNormal;

	if (showNormal) {
		auto normalProgram = Shader::getInstance()->getShaderProgram("normal");
		glUseProgram(normalProgram);

		camera->putCameraUniform("normal");

		circle->Draw("normal");
		light->Draw("normal");


		//backPack->Draw("normal");
	}

	imguiController->Render();
	SDL_GL_SwapWindow(mWindow);
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
