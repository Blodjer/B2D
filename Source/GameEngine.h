#pragma once

class GameEngine
{
	GameEngine();
public:
	~GameEngine();

	static GameEngine* Instance() {
		static GameEngine GameEngineI;
		return &GameEngineI;
	}

	static GameEngine* const Init();
	void Start();
	void Shutdown();

private:
	void GameLoop();
	void HandleEvents();
	void Tick(float deltaTime);
	void LimitFps();
	void Draw(class Graphics* graphics);

	void CleanUp();

public:
	class Graphics* const GetGraphicsInstance() const { return this->GraphicsI; }
	class GameInstance* const GetGameInstance() const { return this->GameI; }

private:
	bool PendingShutdown = false;

	class Graphics* GraphicsI;
	class GameInstance* GameI;
};