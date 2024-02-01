#include "Game.h"
Game::Game() {
	mWindow = nullptr;
	mIsRunning = true;

	paddleH = 100;
	thickness = 15;

	paddles.push_back({
		10,
		768 / 2.0f - paddleH / 2.0f
	});

	paddles.push_back({
		1024 - 10,
		768 / 2.0f - paddleH / 2.0f
	});

	mLeftPaddleKeys.push_back(SDL_SCANCODE_W);
	mLeftPaddleKeys.push_back(SDL_SCANCODE_S);
	mLeftPaddleKeys.push_back(SDL_SCANCODE_I);
	mLeftPaddleKeys.push_back(SDL_SCANCODE_K);

	balls.push_back({
		1024/2.0f,
		768/2.0f
	});
	balls.push_back({
		1024 / 2.0f,
		768 / 2.0f - 15.0f
	});
	balls.push_back({
		1024 / 2.0f,
		768 / 2.0f -40.0f
	});
	balls.push_back({
		1024 / 2.0f,
		768 / 2.0f - 22.0f
	});
	balls.push_back({
		1024 / 2.0f,
		768 / 2.0f + 55.0f
	});

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

	// 패들 이동 감지

	for (int i = 0; i < paddles.size(); i++) {
		auto& paddle = paddles[i];
		paddle.mPaddleDir = 0;
		if (state[mLeftPaddleKeys[i * 2]]) {
			paddle.mPaddleDir -= 1;
		}
		if (state[mLeftPaddleKeys[i * 2 + 1]]) {
			paddle.mPaddleDir += 1;
		}
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

	for (auto& paddle : paddles) {
		auto& mPaddlePos = paddle.mPaddlePos;
		auto mPaddleDir = paddle.mPaddleDir;

		if (paddle.mPaddleDir != 0) {
			paddle.mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

			if (paddle.mPaddlePos.y < thickness) {
				mPaddlePos.y = thickness;
			} else if (paddle.mPaddlePos.y > (768.0f - paddleH)) {
				mPaddlePos.y = 768.0f - paddleH;
			}
		}
	}

	for (auto& ball : balls) {
		auto& mBallPos = ball.mBallPos;
		auto& mBallVel = ball.mBallVel;

		mBallPos.x += mBallVel.x * deltaTime;
		mBallPos.y += mBallVel.y * deltaTime;

		// 공이 벽에 부딪히면 반사
		if (mBallPos.y <= thickness && mBallVel.y < 0.0f) {
			mBallVel.y *= -1;
		}
		else if (mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f) {
			mBallVel.y *= -1;
		}

		for (auto& paddle : paddles) {
			auto& mPaddlePos = paddle.mPaddlePos;

			glm::vec2 paddleLeftTop = glm::vec2(
				mPaddlePos.x,
				mPaddlePos.y
			);

			glm::vec2 paddleRightBottom = glm::vec2(
				mPaddlePos.x + thickness,
				mPaddlePos.y + paddleH
			);

			glm::vec2 ballLeftTop = glm::vec2(
				mBallPos.x,
				mBallPos.y
			);

			glm::vec2 ballRightBottom = glm::vec2(
				mBallPos.x + thickness,
				mBallPos.y + thickness
			);

			if (paddleLeftTop.x > ballLeftTop.x &&
				paddleLeftTop.x < ballRightBottom.x &&
				paddleLeftTop.y < ballRightBottom.y &&
				paddleRightBottom.y > ballLeftTop.y
				) {
				mBallVel.x *= -1;
			}

			if (mBallPos.x <= 0) {
				//mIsRunning = false;
			}
		}
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

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_Rect wall{
		0,
		0,
		1024,
		thickness
	};

	SDL_RenderFillRect(mRenderer, &wall);

	SDL_Rect bottomWall{
		0,
		768 - thickness,
		1024,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &bottomWall);

	for (auto& ball : balls) {
		auto& mBallPos = ball.mBallPos;

		SDL_Rect ball
		{
			static_cast<int>(mBallPos.x - thickness / 2),
			static_cast<int>(mBallPos.y - thickness / 2),
			thickness,
			thickness
		};

		SDL_RenderFillRect(mRenderer, &ball);
	}

	for (auto& paddle : paddles) {
		auto& mPaddlePos = paddle.mPaddlePos;

		SDL_Rect paddle
		{
			static_cast<int>(mPaddlePos.x - thickness / 2),
			static_cast<int>(mPaddlePos.y - thickness / 2),
			thickness,
			paddleH
		};

		SDL_RenderFillRect(mRenderer, &paddle);
	}

	SDL_RenderPresent(mRenderer);
}
