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

void Game::GenerateOutput() {
	imguiController->Update();

	float timeValue = SDL_GetTicks() / 1000.0f;

	// 리셋해줘야함!
	meshRenderer->ResetMesh();

	meshRenderer->AddMesh(backPack);
	meshRenderer->MeshAlignment(camera.get());

	lightManager->MakeShadow(meshRenderer);

	postProcessingFrameBuffer->use();

	// TODO: simple-shading -> default 로 변경
	
	// const char* shaderName = "simple-shading";
	const char* shaderName = "default";

	cubeMap->PutCubeMapTexture(shaderName);
	camera->putCameraUniform(shaderName);
	lightManager->PutLightUniform(shaderName);
	meshRenderer->Draw(shaderName);


	lightManager->DrawLight(camera);
	cubeMap->Draw("cubemap", camera.get());

	auto normalMode = ImguiController::getInstance()->showNormal;

	if (normalMode) {
		lightManager->PutLightUniform("normal");
		camera->putCameraUniform("normal");
		meshRenderer->Draw("normal");
	}

	postProcessingFrameBuffer->Draw("framebuffer-example");

	imguiController->Render();
	SDL_GL_SwapWindow(mWindow);
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
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
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

	// 화면에 그릴 오브젝트들 생성
	plane = make_shared<Plane>();
	plane->scale = glm::vec3(5.0f, 5.0f, 5.0f);
	plane->position = glm::vec3(0.0f, 0.0f, -2.0f);
	plane->SetTexture("./assets/images/bricks2.jpg", "albedo");
	plane->SetTexture("./assets/images/bricks2_normal.jpg", "normal");
	plane->SetTexture("./assets/images/bricks2_disp.jpg", "height");
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
		box->isAlphaMesh = false;
	}

	for (int i = 0; i < translations.size(); i++) {
		auto grass = make_shared<Plane>();
		grass->SetTexture("./assets/images/grass.png", "albedo");
		grass->SetupMesh();
		grass->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		grass->position = translations[i] + glm::vec3(0.0f,0.5f,0.6f);

		this->grass.push_back(grass);
		grass->isAlphaMesh = true;
	}

	auto _circle = make_shared<Circle>();
	_circle->SetTexture("./assets/images/white.png", "albedo");
	_circle->SetupMesh();



	this->circle.push_back(_circle);

	//backPack = make_shared<Model>("./assets/zeldaPosed001/zeldaPosed001.fbx");
	backPack = make_shared<Model>("./assets/pbrSponza/sponza/Sponza.gltf");
	//backPack = make_shared<Model>("./assets/abandoned_warehouse/scene.gltf");
	//backPack->SetScale(glm::vec3(0.01f, 0.01f, 0.01f));

	camera = make_shared<Camera>(
		90.0f,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);

	camera->cameraPos = glm::vec3(0.0f, 1.0f, 4.0f);

	lightManager = LightManager::getInstance();

	auto imguiController = ImguiController::getInstance();
	
	lightManager->CreateLight
	(
		0,
		//y가 20까지 충분히 뒤로가야함. 안그럼 버그같은 현상이 발생함
		glm::vec3(0, 20, -0.001),
		glm::vec3(0, -1, 0.005),
		12
	);
	imguiController->directionalLightPosition = lightManager->directionLights[0]->box->position;
	imguiController->directionalLightDirection = lightManager->directionLights[0]->direction;
	imguiController->directionalLightDepthMap = lightManager->directionLights[0]->shadow->depthMap;
	
	

	// 포인트.
	
	
	/*
	lightManager->CreateLight
	(
		1,
		//glm::vec3(0.0f, 3.0f, 5.0f),
		glm::vec3(-3, 3, -0.05),
		glm::vec3(-0.042, -0.390, 0.952),
		12
	);
	*/

	std::cout << lightManager->getTotalLightCount() << std::endl;

	input = Input::GetInstance();

	return true;
}

void Game::Shutdown() {
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	SDL_GL_DeleteContext(context);
	delete imguiController;
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

	auto imguiController = ImguiController::getInstance();

	const auto exposure = imguiController->exposure;

	postProcessingFrameBuffer->exposure = exposure;

	lightManager->directionLights[0]->setPosition(imguiController->directionalLightPosition);
	lightManager->directionLights[0]->direction = imguiController->directionalLightDirection;

	//lightManager->UpdateLight(deltaTime);
	//lightManager->directionLights[0]->lookHere(glm::vec3(0.0f, 0.0f, 0.0f));
	/*
	lightManager->lights[0]->setPosition(camera->cameraPos);
	lightManager->lights[0]->direction = camera->cameraFront;
	lightManager->lights[0]->CalculateLightSpaceMatrix();
	*/
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

	if (state[SDL_SCANCODE_H]) {
		std::cout << "depth mode 클릭됨" << std::endl;
		//depthMode != depthMode;
	}

	if (state[SDL_SCANCODE_F]) {
		camera->ResetPitch();
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
		"./shader/framebuffer-example-vertex.glsl",
		"./shader/framebuffer-example-fragment.glsl",
		"framebuffer-example"
	);

	shader->loadShaderProgram(
		"./shader/cubemap-vertex.glsl",
		"./shader/cubemap-fragment.glsl",
		"cubemap"
	);

	shader->loadShaderProgram(
		"./shader/shadow-vertex.glsl",
		"./shader/shadow-fragment.glsl",
		"shadow"
	);

	shader->loadShaderProgram(
		"./shader/shadow-debug-vertex.glsl",
		"./shader/shadow-debug-fragment.glsl",
		"shadow-debug"
	);

	shader->loadShaderProgram(
		"./shader/point-shadow-vertex.glsl",
		"./shader/point-shadow-fragment.glsl",
		"point-shadow",
		"./shader/point-shadow-geometry.glsl"
	);

	shader->loadShaderProgram(
		"./shader/simple-shading-vertex.glsl",
		"./shader/simple-shading-fragment.glsl",
		"simple-shading"
	);
}
