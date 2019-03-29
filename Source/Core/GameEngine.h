#pragma once

#include "Application.h"

class IPlatformApplicationInterface;
class CGameInstance;
class CRenderer;
class CViewport;
class CWindow;

class CGameEngine
{
public:
	CGameEngine(ApplicationConfig const config);

	static CGameEngine* const Instance() {
		return sInstance;
	}

	static CGameEngine* sInstance;

private:
	CGameEngine(const CGameEngine&) = delete;
	void operator=(const CGameEngine&) = delete;

public:
	~CGameEngine();

	void Run();

	void RequestShutdown();

public:
	CGameInstance* const GetGameInstance() const { return mGameInstance; }
	CRenderer* const GetGraphicsInstance() const { return mGraphicsInstance; }
	CWindow* const GetMainWindow() const { return mMainWindow; }

private:
	bool mPendingShutdown = false;

	ApplicationConfig const mConfig;

    IPlatformApplicationInterface* mPlatformApplication = nullptr;
	CWindow* mMainWindow = nullptr;

	CGameInstance* mGameInstance = nullptr;
	CRenderer* mGraphicsInstance = nullptr;
};