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
#include <glm/gtc/quaternion.hpp>
#include <random>


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

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


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
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);

	ImguiController::CreateInstance(mWindow, context);
	imguiController = ImguiController::getInstance();

	stbi_set_flip_vertically_on_load(true);

	// 쉐이더 생성
	CreateShaderProgram();
	meshRenderer = make_shared<MeshRenderer>();

	postProcessingFrameBuffer = make_shared<PostProcessingFrameBuffer>();
	cubeMap = make_shared<CubeMap>("./assets/skybox-radiance/");
	//cubeMap = make_shared<CubeMap>("./assets/skybox/");

	// 화면에 그릴 오브젝트들 생성
	plane = make_shared<Plane>();
	plane->scale = glm::vec3(5.0f, 5.0f, 5.0f);
	plane->position = glm::vec3(0.0f, 0.0f, -2.0f);
	plane->SetTexture("./assets/images/wall.jpg", "albedo");
	plane->SetTexture("./assets/container2_specular.png", "specular");
	plane->SetupMesh();

	plane->rotation = glm::vec3(-90.0f, 0.0f, 0.0f);

	vector<glm::vec3> translations = {
		glm::vec3(-1.5f,0.0f,-0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	for (int i = 0; i < translations.size(); i++) {
		auto box = make_shared<Box>();

		box->SetTexture("./assets/container2.png", "albedo");
		box->SetTexture("./assets/container2_specular.png", "specular");
		box->SetupMesh();
		box->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		box->position = translations[i] + glm::vec3(0.0f, 0.5f, 0.0f);

		this->box.push_back(box);
	}

	for (int i = 0; i < translations.size(); i++) {
		auto grass = make_shared<Plane>();
		grass->SetTexture("./assets/images/grass.png", "albedo");
		grass->SetupMesh();
		grass->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		grass->position = translations[i] + glm::vec3(0.0f,0.5f,0.6f);

		this->grass.push_back(grass);
	}

	auto _circle = make_shared<Circle>();
	_circle->SetTexture("./assets/images/white.png", "albedo");
	_circle->SetupMesh();



	this->circle.push_back(_circle);

	//backPack = make_shared<Model>("./assets/zeldaPosed001/zeldaPosed001.fbx");
	backPack = make_shared<Model>("./assets/pbrSponza/sponza/Sponza.gltf");
	//backPack = make_shared<Model>("./assets/abandoned-warehouse/source/Apocalyptic_Warehouse.fbx");
	//backPack = make_shared<Model>("./assets/abandoned_warehouse/scene.gltf");
	//backPack = make_shared<Model>("./assets/abandoned_warehouse/scene.gltf");

	//backPack->scale = glm::vec3(0.05f, 0.05f, 0.05f);
	//backPack->scale = glm::vec3(0.01f, 0.01f, 0.01f);
	//backPack->SetScale(glm::vec3(0.01f, 0.01f, 0.01f));

	camera = make_shared<Camera>(
		45.0f,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);

	camera->cameraPos = glm::vec3(0.0f, 1.0f, 4.0f);

	lightManager = make_shared<LightManager>(3);

	// 태양.
	lightManager->CreateLight
	(
		1,
		glm::vec3(0.0f, 2.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		8
	);
	

	std::cout << lightManager->lights.size() << std::endl;

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

	accTime += deltaTime;

	input->SetMouse();

	lightManager->UpdateLight(deltaTime);
}

void Game::GenerateOutput() {
	imguiController->Update();

	float timeValue = SDL_GetTicks() / 1000.0f;

	postProcessingFrameBuffer->use();

	// 리셋해줘야함!
	meshRenderer->ResetMesh();

	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram("default");
	// 만약 찾지 못하면 -1이다.
	glUseProgram(program);

	camera->putCameraUniform("default");

	shader->setInt("default", "activeLight", activeLight);

	lightManager->PutLightUniform("default");
	/*
	meshRenderer->AddMesh(plane);

	for (int i = 0; i < 5; i++) {
		meshRenderer->AddMesh(box[i]);
	}

	for (int i = 0; i < grass.size(); i++) {
		meshRenderer->AddMesh(grass[i]);
	}
	*/
	meshRenderer->AddMesh(circle[0]);

	meshRenderer->AddMesh(backPack);
	meshRenderer->MeshAlignment(camera.get());

	cubeMap->PutCubeMapTexture("default");
	meshRenderer->Draw("default",camera.get());

	lightManager->DrawLight(camera);
	cubeMap->Draw("cubemap", camera.get());

	postProcessingFrameBuffer->Draw("framebuffer-example");

	auto showNormal = imguiController->showNormal;

	if (showNormal) {
		auto normalProgram = Shader::getInstance()->getShaderProgram("normal");
		glUseProgram(normalProgram);

		camera->putCameraUniform("normal");
		//circle->Draw("normal");
		plane->Draw("normal");


		//ㄴ->Draw("normal");
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

void Game::CreateShaderProgram() {
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

	shader->loadShaderProgram(
		"./shader/light-vertex.glsl",
		"./shader/light-fragment.glsl",
		"light"
	);

	shader->loadShaderProgram(
		"./shader/stencil-layout-vertex.glsl",
		"./shader/stencil-layout-fragment.glsl",
		"stencil-layout"
	);

	shader->loadShaderProgram(
		"./shader/framebuffer-example-vertex.glsl",
		"./shader/framebuffer-example-fragment.glsl",
		"framebuffer-example"
	);

	shader->loadShaderProgram(
		"./shader/cubemap-vertex.glsl",
		"./shader/cubemap-fragment.glsl",
		"cubemap"
	);
}
