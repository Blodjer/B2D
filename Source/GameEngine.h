#pragma once

#include "Platform/PlatformMessageHandlerInterface.h"
#include "Application.h"
#include "Engine/ModuleManager.h"

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

    static GameEngine* sInstance;

public:
    static void Create(ApplicationConfig const& config);
    void Init();
    static void Shutdown();

	static GameEngine* const Instance() {
        B2D_ASSERT(sInstance != nullptr);
		return sInstance;
	}

public:
	void Run();

	void RequestShutdown();

public:
    IPlatformApplicationInterface* GetPA() const { return mPA; }

    IGraphicsHardwareInterface* GetGHI() const { return mGHI; }

    ModuleManager* const GetModuleManager() { return &mModuleManager; }
    RenderManger* GetRenderManager() const { return mRenderManager; }

	CGameInstance* GetGameInstance() const { return mGameInstance; }
    GenericWindow* GetMainWindow() const { return mMainWindow; }

private:
    virtual bool OnKeyEvent(GenericWindow* window, int32 scancode, EKey key, EKeyEvent event) override;
    virtual bool OnMouseMove(GenericWindow* window, TVec2 position) override;
    virtual bool OnMouseButton(GenericWindow* window, EMouseButton button, EMouseButtonEvent event) override;

private:
	bool mPendingShutdown = false;

	ApplicationConfig const mConfig;

    IPlatformApplicationInterface* mPA = nullptr;
    IGraphicsHardwareInterface* mGHI = nullptr;

    ModuleManager mModuleManager;
    RenderManger* mRenderManager = nullptr;

    GenericWindow* mMainWindow = nullptr;
	CGameInstance* mGameInstance = nullptr;

};