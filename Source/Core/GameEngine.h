#pragma once

#include "Application.h"

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
	void Shutdown();

private:
	void GameLoop();
	void HandleEvents();
	void Tick(float deltaTime);
	void Draw(CViewport const* const viewport);

	void CleanUp();

public:
	CGameInstance* const GetGameInstance() const { return mGameInstance; }
	CRenderer* const GetGraphicsInstance() const { return mGraphicsInstance; }
	CWindow* const GetWindow() const { return mWindow; }

private:
	bool mPendingShutdown = false;

	ApplicationConfig const mConfig;

	CGameInstance* mGameInstance;
	CRenderer* mGraphicsInstance;
	CWindow* mWindow;
};