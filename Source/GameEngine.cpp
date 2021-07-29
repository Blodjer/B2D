#include "B2D_pch.h"
#include "GameEngine.h"

#include "Core/Core.h"
#include "Editor/EditorModule.h"
#include "Editor/View/WorldEditorView.h"
#include "Game/GameInstance.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/RenderManager.h"
#include "Graphics/ShaderLibrary.h"
#include "Input/Input.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformApplication.h"
#include "Platform/PlatformInterface.h"

#include <GLFW/glfw3.h>

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

    m_moduleManager.Shutdown();

    if (m_renderManager)
    {
        m_renderManager->Shutdown();
    }
    if (m_GHI)
    {
        m_GHI->Shutdown();
    }
    if (m_PAI)
    {
        m_PAI->Shutdown();
    }

    delete m_renderManager;
    delete m_GHI;
    delete m_PAI;
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
    B2D_LOG_INFO("Initialize engine...");

    ApplicationConfig::Dump(m_config);
    UMath::RandomInit(static_cast<unsigned int>(time(nullptr)));

    // Create Platform Application Interface
    m_PAI = new PlatformApplication();
    B2D_ASSERT(m_PAI->Init());
    m_PAI->AddMessageHandler(this);

    // Create Graphics Hardware Interface
    m_GHI = m_PAI->CreateGHI();

    // Init Graphics Hardware Interface
    if (m_GHI)
    {
        ShaderLibrary::PreloadAll();
        B2D_ASSERT(m_GHI->Init());
    }
    else
    {
        B2D_LOG_WARNING("No GHI was created!");
    }

    // Create Main Window
    m_mainWindow = m_PAI->MakeWindow(m_config.windowWidth, m_config.windowHeight, m_config.name);
    B2D_ASSERT(m_mainWindow);

    // Create Render Manager
    m_renderManager = new RenderManager();
    m_renderManager->Init(m_config.multithread);

#if 1 // Load Editor
    m_moduleManager.Load<EditorModule>();
#endif

    // TODO: GameInstance should only be valid while the game is running. Should be null in editor mode.
    m_gameInstance = new CGameInstance(m_mainWindow);

    B2D_LOG_INFO("Engine initilized!\n");
}

void GameEngine::Shutdown()
{
    delete ms_instance;
}

void GameEngine::Run()
{
    double const evaluationDuration = 1.0;
	double lastTickTime = glfwGetTime();
	double nextEvalutationTime = glfwGetTime() + evaluationDuration;
	uint32 frames = 0;

	while (!GetMainWindow()->ShouldClose())
	{
        double const nowTime = glfwGetTime();
		float const deltaTime = static_cast<float>(nowTime - lastTickTime);
		lastTickTime = nowTime;

        if (nextEvalutationTime < nowTime)
		{
			m_fps = static_cast<uint32>(UMath::Floor(frames / evaluationDuration));
			frames = 0;
			nextEvalutationTime = nowTime + evaluationDuration;
		}

        Input::Flush();
        m_PAI->PollEvents();

        m_moduleManager.ForwardBeginFrame();

        if (Input::IsKey(EKey::ESCAPE, EKeyEvent::Press))
        {
            GameEngine::Instance()->RequestShutdown();
        }
        
        static bool pause = false;
        if (Input::IsKey(EKey::P, EKeyEvent::Press))
        {
            pause = !pause;
        }

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
