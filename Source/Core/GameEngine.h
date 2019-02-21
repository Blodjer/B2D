#pragma once

class CGameInstance;
class CRenderer;
class CViewport;
class CWindow;

class CGameEngine
{
private:
	CGameEngine();
	CGameEngine(const CGameEngine&) = delete;
	void operator=(const CGameEngine&) = delete;

public:
	~CGameEngine();

	static CGameEngine* const Instance() {
		static CGameEngine msGameEngine;
		return &msGameEngine;
	}

	static CGameEngine* const Init();
	void Start();
	void Shutdown();

private:
	void GameLoop();
	void HandleEvents();
	void Tick(float deltaTime);
	void Draw(CViewport const* const viewport, CRenderer* const graphics);

	void CleanUp();

public:
	CGameInstance* const GetGameInstance() const { return mGameInstance; }
	CRenderer* const GetGraphicsInstance() const { return mGraphicsInstance; }
	CWindow* const GetWindow() const { return mWindow; }

private:
	bool mPendingShutdown = false;

	CGameInstance* mGameInstance;
	CRenderer* mGraphicsInstance;
	CWindow* mWindow;
};