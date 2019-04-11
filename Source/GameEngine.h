#pragma once

#include "Platform/PlatformMessageHandlerInterface.h"
#include "Application.h"

class CGameInstance;
class CRenderer;
class CViewport;
class GenericWindow;
class IGraphicsHardwareInterface;
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
    void Init();
    static void Shutdown();

	static CGameEngine* const Instance() {
        B2D_ASSERT(sInstance != nullptr);
		return sInstance;
	}

public:
	void Run();

	void RequestShutdown();

public:
    IPlatformApplicationInterface* GetPA() const { return mPA; }

    IGraphicsHardwareInterface* GetGHI() const { return mGHI; }
	CRenderer* GetRenderer() const { return mRenderer; }

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

    GenericWindow* mMainWindow = nullptr;

	CGameInstance* mGameInstance = nullptr;
	CRenderer* mRenderer = nullptr;
};