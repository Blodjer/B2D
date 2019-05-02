#include "B2D_pch.h"
#include "GameEngine.h"

#include "Core/Core.h"
#include "Editor/EditorModule.h"
#include "Game/GameInstance.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/RenderManger.h"
#include "Input/Input.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformApplication.h"
#include "Platform/PlatformInterface.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

CGameEngine* CGameEngine::sInstance = nullptr;

CGameEngine::CGameEngine(ApplicationConfig const& config)
    : mConfig(config)
{
}

CGameEngine::~CGameEngine()
{
    delete mGameInstance;

    // TODO: replace by smart pointer
    // TODO: replace by base (init, shutdown,...)

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

    mRenderManager = new RenderManger();
    mRenderManager->Init(false);

#if 1 // Load Editor
    mModuleManager.Load<EditorModule>();
    //mPA->AddMessageHandler(mEditor);
#endif

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

        mModuleManager.ForwardBeginFrame();
        
        static bool p_open = true;
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("Example: Simple overlay", &p_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("Fps:   %d", fps);
            ImGui::Text("Delta: %.2fms", 1000.0f / fps);
        }
        ImGui::End();

        if (Input::IsKey(EKey::ESCAPE, EKeyEvent::Press))
            CGameEngine::Instance()->RequestShutdown();

		// Tick
        std::chrono::time_point<clock> start = clock::now();
        mGameInstance->Tick(deltaTime);
		duration ChronoTick = clock::now() - start;

        mModuleManager.ForwardTick(deltaTime);

        mRenderManager->Render();

        mModuleManager.ForwardEndFrame();
        
        mRenderManager->Draw();

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
