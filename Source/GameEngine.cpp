#include "B2D_pch.h"
#include "GameEngine.h"

#include "Core/Core.h"
#include "Game/GameInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Viewport.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Input/Input.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformApplication.h"
#include "Platform/PlatformInterface.h"

#include <GLFW/glfw3.h>

CGameEngine* CGameEngine::sInstance = nullptr;

CGameEngine::CGameEngine(ApplicationConfig const& config)
    : mConfig(config)
{
}

CGameEngine::~CGameEngine()
{
    delete mGameInstance;
    delete mRenderer;

    // TODO: replace by smart pointer
    mPA->Shutdown();
    delete mPA;

    mGHI->Shutdown();
    delete mGHI;
}

void CGameEngine::Create(ApplicationConfig const& config)
{
    if (B2D_CHECK(sInstance != nullptr))
    {
        return;
    }

    sInstance = new CGameEngine(config);
    sInstance->Init();
}

void CGameEngine::Init()
{
    B2D_CORE_INFO("Initialize engine...");

    ApplicationConfig::Dump(mConfig);
    UMath::RandomInit(static_cast<unsigned int>(time(nullptr)));

    // Platform Initialization
    mPA = new PlatformApplication();
    B2D_ASSERT(mPA->Init());
    mPA->AddMessageHandler(this);

    // Create Main Window
    mMainWindow = mPA->MakeWindow(mConfig.windowWidth, mConfig.windowHeight, mConfig.name);

    // Load Graphics Hardware Interface
    mGHI = mPA->CreateGHI();
    B2D_ASSERT(mGHI->Init());

    mRenderer = new CRenderer(mGHI);

    // TODO: GameInstance should only be valid while the game is running. Should be null in editor mode.
    mGameInstance = new CGameInstance(mMainWindow);

    B2D_CORE_INFO("Engine initilized!\n");
}

void CGameEngine::Shutdown()
{
    delete sInstance;
}

void CGameEngine::Run()
{
#if defined(B2D_PLATFORM_WINDOWS) && !defined(B2D_NO_LOGGING)
	HANDLE pOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	using duration = std::chrono::duration<double, std::milli>;
	using clock = std::chrono::high_resolution_clock;

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
        mPA->PollEvents();

        if (Input::IsKey(EKey::F5, EKeyEvent::Press))
            CShader::ReloadAll();

        if (Input::IsKey(EKey::ESCAPE, EKeyEvent::Press))
            CGameEngine::Instance()->RequestShutdown();
        
        //GetMainWindow()->MakeContextCurrent();
        //GetMainWindow()->GetViewport()->Use(); // move to renderer draw call

		// Tick
        std::chrono::time_point<clock> start = clock::now();
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

bool CGameEngine::OnKeyEvent(GenericWindow* window, int32 scancode, EKey key, EKeyEvent event)
{
    Input::OnKey(key, event);
    return false;
}

bool CGameEngine::OnMouseMove(GenericWindow* window, TVec2 position)
{
    return false;
}

bool CGameEngine::OnMouseButton(GenericWindow* window, EMouseButton button, EMouseButtonEvent event)
{
    return false;
}
