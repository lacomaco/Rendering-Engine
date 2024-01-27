#include "./Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include "../Input/Input.h"
#include "../Component/BoxComponent.h"
#include "../System/BoxDrawSystem.h"
#include "../Component/MoveComponent.h"
#include "../Component/KeyboardControlledComponent.h"
#include "../System/PaddleMoveSystem.h"
#include "../System/BallMoveSystem.h"

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
    isRunning = false;
    isDebug = false;
    registry = std::make_unique<Registry>();
    Logger::Log("Game constructor called!");
}

Game::~Game() {
    Logger::Log("Game destructor called!");   
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL.");
        return;
    }
    if (TTF_Init() != 0) {
        Logger::Err("Error initializing SDL TTF.");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        Logger::Err("Error creating SDL window.");
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        Logger::Err("Error creating SDL renderer.");
        return;
    }

    // Initialize the ImGui context
    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    
    isRunning = true;
}

void Game::ProcessInput() {
    if (Input::GetKey(KeyCode::ESCAPE)) {
		isRunning = false;
	}
    if (Input::GetKey(KeyCode::F1)) {
		isDebug = !isDebug;
	}
}

void Game::Setup() {
    registry->AddSystem<BoxDrawSystem>();
    registry->AddSystem<PaddleMoveSystem>();
    registry->AddSystem<BallMoveSystem>();

    auto paddle = registry->CreateEntity();
    const int thickness = 15;
    const int paddleHeight = 100;
    int screenWidth, screenHeight;
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);

    paddle.AddComponent<BoxComponent>(thickness, paddleHeight);
    paddle.AddComponent<MoveComponent>(20, screenHeight / 2 - paddleHeight / 2);
    paddle.AddComponent<KeyboardControlledComponent>();
    paddle.Tag("paddle");

    auto ball = registry->CreateEntity();
    ball.AddComponent<BoxComponent>(10, 10);
    ball.AddComponent<MoveComponent>(screenWidth/2 - 5, screenHeight/2 - 5,glm::vec2(10.0f,10.0f));
    ball.Tag("ball");
}

void Game::Update() {
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    millisecsPreviousFrame = SDL_GetTicks();

    registry->Update();
    registry->GetSystem<PaddleMoveSystem>().Update(deltaTime, Game::windowHeight);

}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
    registry->GetSystem<BoxDrawSystem>().Update(renderer);
    SDL_RenderPresent(renderer);
}

void Game::Run() {
    Setup();
    while (isRunning) {
        Input::ProcessInput();
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy() {
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
