#include "Game.h"
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
		0 // 추후 SDL_WINDOW_OPENGL 플래그로 변경할 예정
	);

	if (!mWindow) {
		SDL_Log("윈도우 생성 실패 %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	return true;
}

void Game::Shutdown() {
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
	SDL_DestroyRenderer(mRenderer);
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
	SDL_SetRenderDrawColor(
		mRenderer,
		0,
		0,
		255,
		255
	);
	SDL_RenderClear(mRenderer);


	SDL_RenderPresent(mRenderer);
}
