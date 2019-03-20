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

    World* GetWorld() { return mWorld; }

	static CPlayerController* AddPlayerController(uint32 id);
	static void RemovePlayerController(uint32 id);
	static const std::map<uint32, CPlayerController*>* const GetPlayerControllers() { return &CGameInstance::mPlayerControllers; }

private:
	void HandleInput(uint32 event);
	void Tick(float deltaTime);

private:
    World* mWorld = nullptr;
	static std::map<uint32, CPlayerController*> mPlayerControllers;
};

