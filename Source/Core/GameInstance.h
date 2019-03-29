#pragma once

#include "Core/Core.h"

class LocalPlayer;
class CWindow;
class World;

class CGameInstance
{
	friend CGameEngine;

	CGameInstance(CWindow* const owningWindow);
public:
	~CGameInstance();

    World* GetWorld() { return mWorld; }
    CWindow* const GetWindow() { return mWindow; }

	LocalPlayer* AddLocalPlayer();
	void RemoveLocalPlayer(uint32 const playerId);

    LocalPlayer* GetLocalPlayer(uint32 const playerId) const;
    std::vector<LocalPlayer*> const& GetLocalPlayers() const { return mLocalPlayers; }

private:
	void Tick(float deltaTime);

private:
    World* mWorld = nullptr;
    CWindow* const mWindow = nullptr;

    std::vector<LocalPlayer*> mLocalPlayers;
    uint32 mLocalPlayerIdCount = 0;

    //InputDeviceManager
    //  std::unordered_set<uint32> mAvailableDevices
    //  OnDeviceConnected()
    //  OnDeviceDisconnected()

    //PlayerInputManager mPlayerInputManager;
    //  std::unordered_map<uint32, LocalPlayer*> mInputToPlayerMap;
};

