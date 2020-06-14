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
#include "Core/Profiler.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "Editor/View/WorldEditorView.h"
#include "Game/Core/SystemAdmin.h"
#include "Game/Core/World.h"

GameEngine* GameEngine::ms_instance = nullptr;

GameEngine::GameEngine(ApplicationConfig const& config)
    : m_config(config)
{
}

GameEngine::~GameEngine()
{
    delete m_gameInstance;

    // TODO: replace by smart pointer
    // TODO: replace by base (init, shutdown,...)

    m_PA->Shutdown();
    m_GHI->Shutdown();
    m_renderManager->Shutdown();

    delete m_PA;
    delete m_GHI;
    delete m_renderManager;
}

void GameEngine::Create(ApplicationConfig const& config)
{
    if (B2D_CHECK(ms_instance != nullptr))
    {
        return;
    }

    ms_instance = new GameEngine(config);
    ms_instance->Init();
}

void GameEngine::Init()
{
    B2D_CORE_INFO("Initialize engine...");

    ApplicationConfig::Dump(m_config);
    UMath::RandomInit(static_cast<unsigned int>(time(nullptr)));

    // Platform Initialization
    m_PA = new PlatformApplication();
    B2D_ASSERT(m_PA->Init());
    m_PA->AddMessageHandler(this);

    // Create Main Window
    m_mainWindow = m_PA->MakeWindow(m_config.windowWidth, m_config.windowHeight, m_config.name);

    // Load Graphics Hardware Interface
    m_GHI = m_PA->CreateGHI();
    B2D_ASSERT(m_GHI->Init());

    m_renderManager = new RenderManger();
    m_renderManager->Init(m_config.multithread);

#if 1 // Load Editor
    m_moduleManager.Load<EditorModule>();
    //mPA->AddMessageHandler(mEditor);
#endif

    // TODO: GameInstance should only be valid while the game is running. Should be null in editor mode.
    m_gameInstance = new CGameInstance(m_mainWindow);

    B2D_CORE_INFO("Engine initilized!\n");
}

void GameEngine::Shutdown()
{
    delete ms_instance;
}

void GameEngine::Run()
{
	//using duration = std::chrono::duration<double, std::milli>;
	//using clock = std::chrono::high_resolution_clock;

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
        m_PA->PollEvents();

        m_moduleManager.ForwardBeginFrame();

        static bool p_open = true;
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("Example: Simple overlay", &p_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("Fps:   %d", fps);
            ImGui::Text("Delta: %.2fms", 1000.0f / fps);
        }
        ImGui::End();

        if (Input::IsKey(EKey::ESCAPE, EKeyEvent::Press))
            GameEngine::Instance()->RequestShutdown();

        if (Input::IsKey(EKey::V, EKeyEvent::Press))
            GetModuleManager()->Get<EditorModule>()->CreateEditorView<WorldEditorView>();
        
        static bool pause = false;
        if (Input::IsKey(EKey::P, EKeyEvent::Press))
            pause = !pause;

		// Tick
        if (!pause || Input::IsKey(EKey::O, EKeyEvent::Press))
        {
            m_gameInstance->Tick(deltaTime);
        }

        m_renderManager->Tick(deltaTime);

        m_moduleManager.ForwardTick(deltaTime);

        m_moduleManager.ForwardEndFrame();
        
        m_renderManager->Draw();

		frames++;
	}
}

void GameEngine::RequestShutdown()
{
	GetMainWindow()->SetShouldClose(true);
	m_pendingShutdown = true;
}

bool GameEngine::OnKeyEvent(GenericWindow* window, int32 scancode, EKey key, EKeyEvent event)
{
    Input::OnKey(key, event);
    return false;
}

bool GameEngine::OnMouseMove(GenericWindow* window, TVec2 position)
{
    return false;
}

bool GameEngine::OnMouseButton(GenericWindow* window, EMouseButton button, EMouseButtonEvent event)
{
    return false;
}
