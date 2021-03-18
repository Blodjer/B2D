#pragma once

#include "Application.h"
#include "Engine/ModuleManager.h"
#include "Platform/PlatformMessageHandlerInterface.h"

class CGameInstance;
class CViewport;
class Editor;
class GenericWindow;
class IGraphicsHardwareInterface;
class IPlatformApplicationInterface;
class RenderManger;

class GameEngine final : private IPlatformMessageHandlerInterface
{
private:
	GameEngine(ApplicationConfig const& config);
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;
    void operator=(const GameEngine&) = delete;

    static GameEngine* ms_instance;

public:
    static void Create(ApplicationConfig const& config);
    void Init();
    static void Shutdown();

	static GameEngine* const Instance() {
        B2D_ASSERT(ms_instance != nullptr);
		return ms_instance;
	}

public:
	void Run();

	void RequestShutdown();

public:
    IPlatformApplicationInterface* GetPAI() const { return m_PAI; }

    IGraphicsHardwareInterface* GetGHI() const { return m_GHI; }

    ModuleManager* const GetModuleManager() { return &m_moduleManager; }
    RenderManger* GetRenderManager() const { return m_renderManager; }

	CGameInstance* GetGameInstance() const { return m_gameInstance; }
    GenericWindow* GetMainWindow() const { return m_mainWindow; }

    uint32 GetFps() const { return m_fps; }

private:
    virtual bool OnKeyEvent(GenericWindow* window, int32 scancode, EKey key, EKeyEvent event) override;
    virtual bool OnMouseMove(GenericWindow* window, TVec2 position) override;
    virtual bool OnMouseButton(GenericWindow* window, EMouseButton button, EMouseButtonEvent event) override;

private:
	bool m_pendingShutdown = false;
    uint32 m_fps = 0;

	ApplicationConfig const m_config;

    IPlatformApplicationInterface* m_PAI = nullptr;
    IGraphicsHardwareInterface* m_GHI = nullptr;

    ModuleManager m_moduleManager;
    RenderManger* m_renderManager = nullptr;

    GenericWindow* m_mainWindow = nullptr;
	CGameInstance* m_gameInstance = nullptr;

};