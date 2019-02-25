#include "B2D_pch.h"
#include "GameEngine.h"

#include "Core.h"
#include "GameInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Viewport.h"
#include "Graphics/Window.h"
#include "Input.h"

#include <GLFW/glfw3.h>
#include <iostream>

CGameEngine* CGameEngine::sInstance = nullptr;

CGameEngine::CGameEngine(ApplicationConfig const config)
	: mConfig(config)
{
	if (sInstance != nullptr)
	{
		B2D_CORE_ERROR("Engine already initialized");
		return;
	}

	sInstance = this;

	B2D_CORE_INFO("Initialize engine");

	ApplicationConfig::Dump(config);

	UMath::RandomInit(static_cast<unsigned int>(time(nullptr)));

	glfwInit();
	glfwSetErrorCallback([](int error, const char* description)
	{
		B2D_CORE_ERROR("GLFW error {0}: {1}", error, description);
	});

	mWindow = new CWindow(config.windowWidth, config.windowHeight, config.name);
	mGraphicsInstance = new CRenderer();
	mGameInstance = new CGameInstance();

	B2D_CORE_INFO("Engine initilized");
}

CGameEngine::~CGameEngine()
{
	glfwTerminate();
}

void CGameEngine::Run()
{
    B2D_CORE_INFO("Starting game loop");
	GameLoop();
}

void CGameEngine::GameLoop()
{
	CInput input;

#ifdef _DEBUG
	HANDLE pOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	using duration = std::chrono::duration<double, std::milli>;
	using clock = std::chrono::high_resolution_clock;
	std::chrono::time_point<clock> start;

	uint32 iFrames = 0;
	uint32 iFps = 0;

	double fLastTick = glfwGetTime();
	double fNextSecond = glfwGetTime() + 1;
	while (!GetWindow()->ShouldClose())
	{
		GetWindow()->MakeContextCurrent();
		GetWindow()->GetViewport()->Use();

		const double fNow = glfwGetTime();
		const float fDeltaTime = static_cast<float>(fNow - fLastTick);
		fLastTick = fNow;

		if (fNextSecond < fNow)
		{
			iFps = iFrames;
			iFrames = 0;
			fNextSecond = fNow + 1;
		}

		//CShader::ReloadAll();

		// Events
		start = clock::now();
		HandleEvents();
		duration ChronoEvents = clock::now() - start;

		// Tick
		start = clock::now();
		Tick(fDeltaTime);
		duration ChronoTick = clock::now() - start;
		
		// Draw
		start = clock::now();
		Draw(GetWindow()->GetViewport(), mGraphicsInstance);
		duration ChronoDraw = clock::now() - start;
		
#ifdef _DEBUG
		// Debug
		COORD pos = { -1, -1 };
		CONSOLE_SCREEN_BUFFER_INFO cbsi;
		if (GetConsoleScreenBufferInfo(pOutputHandle, &cbsi))
		{
			pos = cbsi.dwCursorPosition;
			SetConsoleCursorPosition(pOutputHandle, { 0, 0 });
		}
		
		printf("Delta: %.2fms | Events: %.2fms | Tick: %.2fms | Draw: %.2fms | Fps: %d               ",
			    fDeltaTime * 1000, ChronoEvents.count(), ChronoTick.count(), ChronoDraw.count(), iFps);
		
		if (pos.X >= 0 && pos.Y >= 0)
		{
			SetConsoleCursorPosition(pOutputHandle, pos);
		}
#endif
		iFrames++;
	}

	CleanUp();
}

// TODO: Move to window
void CGameEngine::HandleEvents()
{
	glfwPollEvents();
}

void CGameEngine::Tick(float deltaTime)
{
	mGameInstance->Tick(deltaTime);
}

void CGameEngine::Draw(CViewport const* const viewport, CRenderer* const graphics)
{
	graphics->Clear();

	mGameInstance->Draw(viewport, graphics);

	mWindow->Swap();
}

void CGameEngine::Shutdown()
{
	GetWindow()->SetShouldClose(true);
	mPendingShutdown = true;
}

void CGameEngine::CleanUp()
{
	delete mWindow;
	delete mGameInstance;
	delete mGraphicsInstance;
}
