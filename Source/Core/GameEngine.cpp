#include "B2D_pch.h"
#include "GameEngine.h"

#include "Core.h"
#include "Core/PlatformApplication.h"
#include "GameInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Viewport.h"
#include "Graphics/Window.h"
#include "Input/Input.h"
#include "PlatformInterface.h"

#include <GLFW/glfw3.h>

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

    mPlatformApplication = new PlatformApplication();
    mPlatformApplication->Init();

	ApplicationConfig::Dump(config);

	UMath::RandomInit(static_cast<unsigned int>(time(nullptr)));
    
    mPlatformApplication->MakeWindow(config.windowWidth, 100, "sgopfksg");
	mMainWindow = mPlatformApplication->MakeWindow(config.windowWidth, config.windowHeight, config.name);

	mGraphicsInstance = new CRenderer();
	mGameInstance = new CGameInstance(mMainWindow);

	B2D_CORE_INFO("Engine initilized");
}

CGameEngine::~CGameEngine()
{
    delete mGameInstance;
    delete mGraphicsInstance;

    // TODO: replace by smart pointer
    mPlatformApplication->Shutdown();
    delete mPlatformApplication;
}

void CGameEngine::Run()
{
#if defined(B2D_PLATFORM_WINDOWS) && !defined(B2D_NO_LOGGING)
	HANDLE pOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	using duration = std::chrono::duration<double, std::milli>;
	using clock = std::chrono::high_resolution_clock;
	std::chrono::time_point<clock> start;

	uint32 frames = 0;
	uint32 fps = 0;

	double lastTick = glfwGetTime();
	double nextSecond = glfwGetTime() + 1;
	while (!GetMainWindow()->ShouldClose())
	{
		const double now = glfwGetTime();
		const float deltaTime = static_cast<float>(now - lastTick);
		lastTick = now;

		if (nextSecond < now)
		{
			fps = frames;
			frames = 0;
			nextSecond = now + 1;
		}

        Input::Flush();
        mPlatformApplication->PollEvents();

        if (Input::IsKey(EKey::F5, EKeyEvent::KEY_DOWN))
            CShader::ReloadAll();

        if (Input::IsKey(EKey::ESCAPE, EKeyEvent::KEY_DOWN))
            CGameEngine::Instance()->RequestShutdown();
        
        GetMainWindow()->MakeContextCurrent();
        GetMainWindow()->GetViewport()->Use(); // move to renderer draw call

		// Tick
		start = clock::now();
        mGameInstance->Tick(deltaTime);
		duration ChronoTick = clock::now() - start;
		
#if defined(B2D_PLATFORM_WINDOWS) && !defined(B2D_NO_LOGGING)
		// Debug
		COORD pos = { -1, -1 };
		CONSOLE_SCREEN_BUFFER_INFO cbsi;
		if (GetConsoleScreenBufferInfo(pOutputHandle, &cbsi))
		{
			pos = cbsi.dwCursorPosition;
			SetConsoleCursorPosition(pOutputHandle, { 0, 0 });
		}
		
		printf("Delta: %.2fms | Fps: %d               ",
			    deltaTime * 1000, fps);
		
		if (pos.X >= 0 && pos.Y >= 0)
		{
			SetConsoleCursorPosition(pOutputHandle, pos);
		}
#endif
		frames++;
	}
}

void CGameEngine::RequestShutdown()
{
	GetMainWindow()->SetShouldClose(true);
	mPendingShutdown = true;
}
