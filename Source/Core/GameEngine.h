#pragma once

#include "PlatformMessageHandlerInterface.h"
#include "Application.h"

class CGameInstance;
class CRenderer;
class CViewport;
class GenericWindow;
class IPlatformApplicationInterface;

class CGameEngine final : private IPlatformMessageHandlerInterface
{
private:
	CGameEngine(ApplicationConfig const& config);
    ~CGameEngine();

    CGameEngine(const CGameEngine&) = delete;
    void operator=(const CGameEngine&) = delete;

    static CGameEngine* sInstance;

public:
    static void Create(ApplicationConfig const& config);
    static void Shutdown();

	static CGameEngine* const Instance() {
        B2D_ASSERT(sInstance != nullptr);
		return sInstance;
	}

public:
    void Init();

	void Run();

	void RequestShutdown();

public:
    IPlatformApplicationInterface* GetPlatformApplication() const { return mPlatformApplication; }
	CGameInstance* GetGameInstance() const { return mGameInstance; }
	CRenderer* GetGraphicsInstance() const { return mGraphicsInstance; }
    GenericWindow* GetMainWindow() const { return mMainWindow; }

private:
	bool mPendingShutdown = false;

	ApplicationConfig const mConfig;

    IPlatformApplicationInterface* mPlatformApplication = nullptr;
    GenericWindow* mMainWindow = nullptr;

	CGameInstance* mGameInstance = nullptr;
	CRenderer* mGraphicsInstance = nullptr;
};