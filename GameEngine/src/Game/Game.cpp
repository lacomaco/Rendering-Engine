#include "Game.h"
#include <SDL.h>
#include <glew.h>
#include "../Util/Shader.h"
#include "../Util/stb_image.h"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "../GameObject/PrimitiveObject/Plane.h"
#include "../Constants.h"
#include <glm/gtc/quaternion.hpp>
#include <random>
#include "../Editor/EditorSharedValue.h"
#include "CameraShareValue.h"



Game::Game() {
	mWindow = nullptr;
	mIsRunning = true;
}

void Game::GenerateOutput() {
	gridGui->Update();

	float timeValue = SDL_GetTicks() / 1000.0f;
	auto shader = Shader::getInstance();

	meshRenderer->ResetMesh();

	if (modelOn) {
		meshRenderer->AddMesh(backPack);
	}
	

	meshRenderer->MeshAlignment(camera.get());
	lightManager->CreateShadow(meshRenderer);


	if (lightManager->GetIsUseSun()) {
		graphicsPipe->DrawGBuffer(
			meshRenderer,
			lightManager->GetSun()->GetLightInfo().position
		);
	}
	else {
		graphicsPipe->DrawGBuffer(meshRenderer);
	}
	graphicsPipe->DrawSSAO();

	if (lightManager->GetIsUseSun()) {
		graphicsPipe->godRays->Draw(
			meshRenderer,
			lightManager->GetSun()->GetLightInfo().position,
			camera,
			graphicsPipe->gBuffer->godRayTexture
		);
	}
	graphicsPipe->use();
	
	const char* shaderName = "default";
	graphicsPipe->PutExposure(shaderName);
	cubeMap->PutCubeMapTexture(shaderName);
	lightManager->PutShadowUniform(shaderName);
	meshRenderer->Draw(shaderName); 
	cubeMap->Draw("cubemap");

	auto normalMode = EditorSharedValue::showNormal;

	if (normalMode) {
		meshRenderer->Draw("normal");
	}

	graphicsPipe->Draw("hdr");

	gridGui->Render();
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
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	gridGui = new GridGui(mWindow, context);

	stbi_set_flip_vertically_on_load(true);

	// 쉐이더 생성
	CreateShaderProgram();
	meshRenderer = make_shared<MeshRenderer>();

	graphicsPipe = make_shared<GraphicsPipeLine>();
	cubeMap = make_shared<CubeMap>("./assets/hdr-cubemap/");

	if (modelOn) {
		backPack = make_shared<Model>("./assets/pbrSponza/sponza/Sponza.gltf");
	}

	camera = make_shared<Camera>(
		CameraShareValue::fov,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);

	{
		camera->bindUBO("normal");
		camera->bindUBO("cubemap");
		camera->bindUBO("default");
		camera->bindUBO("gBuffer");
		camera->bindUBO("light");
		camera->bindUBO("SSAO");
	}

	camera->cameraPos = glm::vec3(-10.008f, 1.144f, -0.001f);

	lightManager = make_shared<LightManager>();
	{
		lightManager->BindLightUBO("default");
		lightManager->BindLightUBO("gBuffer");
	}

	{
		lightManager->BindShadowUBO("cascade-shadow");
		lightManager->BindShadowUBO("default");
	}

	input = Input::GetInstance();

	return true;
}

void Game::Shutdown() {
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	SDL_GL_DeleteContext(context);
	delete gridGui;
}

void Game::UpdateGame() {
	while (!SDL_TICKS_PASSED(SDL_GetTicks(),mTicksCount + 16));

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	mTicksCount = SDL_GetTicks();

	camera->Update(deltaTime);
	lightManager->UpdateLightUBO();
	lightManager->UpdateShadowUBO();

	accTime += deltaTime;

	input->SetMouse();

	const auto exposure = EditorSharedValue::exposure;

	graphicsPipe->exposure = exposure;

	graphicsPipe->bloomThreshold = EditorSharedValue::bloomThreshold;

	cubeMap->select = EditorSharedValue::select;
	cubeMap->lodLevel = EditorSharedValue::lodLevel;

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


		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_F11) {
				EditorSharedValue::editorMode = !EditorSharedValue::editorMode;
				std::cout << "is f12 press out? " << EditorSharedValue::editorMode << std::endl;
				break;
			}
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


void Game::CreateShaderProgram() {
	std::cout << "Shader Compile Start" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

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
		"./shader/hdr-vertex.glsl",
		"./shader/hdr-fragment.glsl",
		"hdr"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/up-sample-fragment.glsl",
		"up-sample"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/down-sample-fragment.glsl",
		"down-sample"
	);

	shader->loadShaderProgram(
		"./shader/cubemap-vertex.glsl",
		"./shader/cubemap-fragment.glsl",
		"cubemap"
	);

	shader->loadShaderProgram(
		"./shader/cascade-shaodw-map-vertex.glsl",
		"./shader/cascade-shadow-map-fragment.glsl",
		"cascade-shadow",
		"./shader/cascade-shadow-map-geometry.glsl"
	);

	shader->loadShaderProgram(
		"./shader/point-shadow-vertex.glsl",
		"./shader/point-shadow-fragment.glsl",
		"point-shadow",
		"./shader/point-shadow-geometry.glsl"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/god-ray-post-processing-fragment.glsl",
		"god-ray-effect"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/lens-flare-fragment.glsl",
		"lensFlare"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/lens-flare-dirt-fragment.glsl",
		"lens-dirt"
	);

	shader->loadShaderProgram(
		"./shader/gbuffer-vertex.glsl",
		"./shader/gbuffer-fragment.glsl",
		"gBuffer"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/ssao-fragment.glsl",
		"SSAO"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/ssao-blur-fragment.glsl",
		"SSAOBlur"
	);

	shader->loadShaderProgram(
		"./shader/diffuse-irradiance-vertex.glsl",
		"./shader/diffuse-irradiance-fragment.glsl",
		"diffuse-irradiance"
	);

	shader->loadShaderProgram(
		"./shader/diffuse-irradiance-vertex.glsl",
		"./shader/pre-filter-environment-fragment.glsl",
		"pre-filter-environment"
	);

	shader->loadShaderProgram(
		"./shader/hdr-vertex.glsl",
		"./shader/brdf-fragment.glsl",
		"brdf-lut"
	);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;

	std::cout << "Shader Compile Complete " << duration.count()  << std::endl;
}
