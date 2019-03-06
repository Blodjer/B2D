#pragma once

#include "Core/Core.h"

class CRenderer;
class CLevel;
class CPlayerController;
class CViewport;
class World;

class CGameInstance
{
	friend CGameEngine;

	CGameInstance();
public:
	~CGameInstance();

	template<class L>
	L* LoadLevel()
	{
        B2D_STATIC_ASSERT_TYPE(CLevel, L);

		if (mLoadedLevel != nullptr)
		{
			delete mLoadedLevel;
		}

		mLoadedLevel = new L();
        return mLoadedLevel;
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
    World* mWorld = nullptr;
	static std::map<uint32, CPlayerController*> mPlayerControllers;
};

