#pragma once

class CGameInstance;
class CGraphics;
struct GLFWwindow;

class CGameEngine
{
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
	void Draw(CGraphics* const graphics);

	void CleanUp();

public:
	CGameInstance* const GetGameInstance() const { return mGameInstance; }
	CGraphics* const GetGraphicsInstance() const { return mGraphicsInstance; }
	GLFWwindow* const GetWindow() const;

private:
	bool mPendingShutdown = false;

	CGameInstance* mGameInstance;
	CGraphics* mGraphicsInstance;
};