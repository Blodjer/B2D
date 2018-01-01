#include "GameEngine.h"
#include <SDL2\SDL.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include "Graphics.h"
#include "BMath.h"
#include "GameInstance.h"
#include "Input.h"

#ifdef _DEBUG
#include "Debug\debug_new.cpp"
#include "Debug.h"
#endif

const int FPS = 240;
const int MAX_FRAME_TIME = 1000 / FPS;

GameEngine::GameEngine()
{
	BMath::RandomInit(time(NULL));
	
	SDL_Init(SDL_INIT_EVERYTHING);

	this->GraphicsI = new Graphics();
	this->GameI = new GameInstance();
}

GameEngine::~GameEngine()
{
	
}

GameEngine* const GameEngine::Init()
{
	return GameEngine::Instance();
}

void GameEngine::Start()
{
	this->GameLoop();

#if DEBUG_MEMORY
	system("cls");
	_CrtDumpMemoryLeaks();
	check_leaks();
	std::cin.get();
#endif
}

void GameEngine::GameLoop()
{
	Input input;

	typedef std::chrono::duration<double, std::milli> duration;
	typedef std::chrono::high_resolution_clock clock;
	std::chrono::time_point<clock> start;

	int iLastTick = SDL_GetTicks();
	while (!this->PendingShutdown)
	{
		const int iNow = SDL_GetTicks();
		float fDeltaTime = (iNow - iLastTick) / 1000.f;
		iLastTick = iNow;
		//printf("%dms      \r", static_cast<int>(fDeltaTime * 1000));

		input.BeginNewFrame();

		start = clock::now();
		this->HandleEvents();
		duration ChronoEvents = clock::now() - start;

		if (this->PendingShutdown)
			break;

		start = clock::now();
		this->Tick(fDeltaTime);
		duration ChronoTick = clock::now() - start;
		
		start = clock::now();
		this->Draw(this->GraphicsI);
		duration ChronoDraw = clock::now() - start;

		if (this->PendingShutdown)
			break;

		printf("Events: %fms | Tick: %fms | Draw: %fms                       \r",
				ChronoEvents.count(), ChronoTick.count(), ChronoDraw.count());

		this->LimitFps();
	}

	this->CleanUp();
}

int iNexTick = 0;
void GameEngine::LimitFps()
{
	int iNow = SDL_GetTicks();
	if (iNexTick > iNow)
		SDL_Delay(iNexTick - iNow);

	iNexTick = iNow + (iNexTick - iNow) + MAX_FRAME_TIME;
}

void GameEngine::HandleEvents()
{
	SDL_PumpEvents();

	SDL_Event aInputEvents[50];
	int iEvents = SDL_PeepEvents(aInputEvents, 50, SDL_GETEVENT, SDL_KEYDOWN, SDL_MULTIGESTURE);

	SDL_Event pEvent[1];
	while (SDL_PeepEvents(pEvent, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT))
	{
		if (pEvent[0].type == SDL_QUIT)
		{
			this->Shutdown();
			return;
		}
	}
	
	for (int i = 0; i < iEvents; i++)
	{
		this->GameI->HandleInput(aInputEvents[i]);
	}
}

void GameEngine::Tick(float fDeltaTime)
{
	this->GameI->Tick(fDeltaTime);
}

void GameEngine::Draw(Graphics* pGraphics)
{
	SDL_Texture* xRenderTarget = SDL_CreateTexture(pGraphics->GetRenderer(), SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, 1024, 720);
	SDL_SetRenderTarget(pGraphics->GetRenderer(), xRenderTarget);
	
	pGraphics->Clear();

	this->GameI->Draw(pGraphics);

	SDL_SetRenderTarget(pGraphics->GetRenderer(), NULL);
	SDL_RenderCopyEx(pGraphics->GetRenderer(), xRenderTarget, NULL, NULL, 0, NULL, SDL_FLIP_NONE);

	pGraphics->Flip();

	SDL_DestroyTexture(xRenderTarget);
}

void GameEngine::Shutdown()
{
	this->PendingShutdown = true;
}

void GameEngine::CleanUp()
{
	delete this->GameI;
	delete this->GraphicsI;

	SDL_Quit();
}
