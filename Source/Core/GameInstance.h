#pragma once

#include "Core/Core.h"

#include <map>

// Only one instance per engine
// Handle Events
// Pause Game

class CRenderer;
class CLevel;
class CPlayerController;
class CViewport;

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

		if (mLoadedLevel != nullptr)
		{
			delete mLoadedLevel;
		}

		mLoadedLevel = new L();
	}

	static CPlayerController* AddPlayerController(uint32 id);
	static void RemovePlayerController(uint32 id);
	static const std::map<uint32, CPlayerController*>* const GetPlayerControllers() { return &CGameInstance::mPlayerControllers; }

private:
	void HandleInput(uint32 event);
	void Tick(float deltaTime);
	void Draw(CViewport const* const viewport, CRenderer* graphics);

private:
	CLevel* mLoadedLevel = nullptr;
	static std::map<uint32, CPlayerController*> mPlayerControllers;
};

