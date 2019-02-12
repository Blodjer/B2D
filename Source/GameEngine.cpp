#include "GameEngine.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "BMath.h"
#include "GameInstance.h"
#include "Graphics.h"
#include "Input.h"
#include "Shader.h"

#include "B2DCore.h"

#include <GLFW\glfw3.h>

CGameEngine::CGameEngine()
{
#if _DEBUG
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 1 });
#endif

	BMath::RandomInit(time(nullptr));

	glfwInit();
	glfwSetErrorCallback([](int error, const char* description)
	{
		std::cerr << "Error: " << description << std::endl;
	});

	mGraphicsInstance = new CGraphics();
	mGameInstance = new CGameInstance();
}

CGameEngine::~CGameEngine()
{
	
}

CGameEngine* const CGameEngine::Init()
{
	return CGameEngine::Instance();
}

void CGameEngine::Start()
{
	GameLoop();

#if B2D_DEBUG_MEMORY
	system("cls");
	_CrtDumpMemoryLeaks();
	check_leaks();
	std::cin.get();
#endif
}

void CGameEngine::GameLoop()
{
	CInput input;

#if _DEBUG
	HANDLE pOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	using duration = std::chrono::duration<double, std::milli>;
	using clock = std::chrono::high_resolution_clock;
	std::chrono::time_point<clock> start;

	int iFrames = 0;
	int iFps = 0;

	double fLastTick = glfwGetTime();
	double fNextSecond = glfwGetTime() + 1;
	while (!glfwWindowShouldClose(GetWindow()))
	{
		const double fNow = glfwGetTime();
		const float fDeltaTime = static_cast<float>(fNow - fLastTick);
		fLastTick = fNow;

		if (fNextSecond < fNow)
		{
			iFps = iFrames;
			iFrames = 0;
			fNextSecond = fNow + 1;
		}

		CShader::ReloadAll();

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
		Draw(mGraphicsInstance);
		duration ChronoDraw = clock::now() - start;
	
#if _DEBUG
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

void CGameEngine::HandleEvents()
{
	glfwPollEvents();

	/*
	for (int i = 0; i < iEvents; i++)
	{
		mGameInstance->HandleInput(aInputEvents[i]);
	}
	*/
}

void CGameEngine::Tick(float deltaTime)
{
	mGameInstance->Tick(deltaTime);
}

void CGameEngine::Draw(CGraphics* const graphics)
{
	graphics->Clear();

	mGameInstance->Draw(graphics);

	graphics->Swap();
}

void CGameEngine::Shutdown()
{
	glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
	mPendingShutdown = true;
}

void CGameEngine::CleanUp()
{
	delete mGameInstance;
	delete mGraphicsInstance;

	glfwTerminate();
}

GLFWwindow* const CGameEngine::GetWindow() const
{
	return mGraphicsInstance->GetWindow();
}
