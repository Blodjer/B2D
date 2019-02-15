#pragma once

#include <map>

// Only one instance per engine
// Handle Events
// Pause Game

class CGameInstance
{
	friend class CGameEngine;

	CGameInstance();
public:
	~CGameInstance();

	template<class L>
	void LoadLevel()
	{
		static_assert(std::is_base_of<CLevel, L>::value, "L must inherit from Scene");

		if (this->LoadedLevel != NULL)
		{
			delete this->LoadedLevel;
		}

		this->LoadedLevel = new L();
	}

	static class CPlayerController* AddPlayerController(int iId);
	static void RemovePlayerController(int iId);
	static const std::map<int, CPlayerController*>* const GetPlayerControllers() { return &CGameInstance::PlayerControllers; }

private:
	void HandleInput(int pEvent);
	void Tick(float fDeltaTime);
	void Draw(class CGraphics* pGraphics);

private:
	class CLevel* LoadedLevel;
	static std::map<int, class CPlayerController*> PlayerControllers;
};

