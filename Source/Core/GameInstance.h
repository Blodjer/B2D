#pragma once

#include "Core/Core.h"

#include <map>

// Only one instance per engine
// Handle Events
// Pause Game

class CGameEngine;
class CGraphics;
class CLevel;
class CPlayerController;

class CGameInstance
{
	friend CGameEngine;

	CGameInstance();
public:
	~CGameInstance();

	template<class L>
	void LoadLevel()
	{
		static_assert(std::is_base_of<CLevel, L>::value, "L must inherit from Scene");

		if (this->LoadedLevel != nullptr)
		{
			delete this->LoadedLevel;
		}

		this->LoadedLevel = new L();
	}

	static CPlayerController* AddPlayerController(uint32 iId);
	static void RemovePlayerController(uint32 iId);
	static const std::map<uint32, CPlayerController*>* const GetPlayerControllers() { return &CGameInstance::PlayerControllers; }

private:
	void HandleInput(uint32 pEvent);
	void Tick(float fDeltaTime);
	void Draw(CGraphics* pGraphics);

private:
	CLevel* LoadedLevel;
	static std::map<uint32, CPlayerController*> PlayerControllers;
};

