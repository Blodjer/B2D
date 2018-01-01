#pragma once

#include <map>

// Only one instance per engine
// Handle Events
// Pause Game

class GameInstance
{
	friend class GameEngine;

	GameInstance();
public:
	~GameInstance();

	template<class L>
	void LoadLevel()
	{
		static_assert(std::is_base_of<Level, L>::value, "L must inherit from Scene");

		if (this->LoadedLevel != NULL)
		{
			delete this->LoadedLevel;
		}

		this->LoadedLevel = new L();
	}

	static class PlayerController* AddPlayerController(int iId);
	static void RemovePlayerController(int iId);

private:
	void HandleInput(union SDL_Event& pEvent);
	void Tick(float fDeltaTime);
	void Draw(class Graphics* pGraphics);

private:
	class Level* LoadedLevel;
	static std::map<int, class PlayerController*> PlayerControllers;
};

