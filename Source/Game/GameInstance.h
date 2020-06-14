#pragma once

#include "Core/Core.h"

class LocalPlayer;
class GenericWindow;
class World;

class CGameInstance
{
	friend GameEngine;

	CGameInstance(GenericWindow* const owningWindow);
public:
	~CGameInstance();

    World* GetWorld() { return m_world; }
    GenericWindow* const GetWindow() { return m_window; }

	LocalPlayer* AddLocalPlayer();
	void RemoveLocalPlayer(uint32 const playerId);

    LocalPlayer* GetLocalPlayer(uint32 const playerId) const;
    std::vector<LocalPlayer*> const& GetLocalPlayers() const { return m_localPlayers; }

private:
	void Tick(float deltaTime);

private:
    World* m_world = nullptr;
    GenericWindow* const m_window = nullptr;

    std::vector<LocalPlayer*> m_localPlayers;
    uint32 m_localPlayerIdCount = 0;

    //InputDeviceManager
    //  std::unordered_set<uint32> mAvailableDevices
    //  OnDeviceConnected()
    //  OnDeviceDisconnected()

    //PlayerInputManager mPlayerInputManager;
    //  std::unordered_map<uint32, LocalPlayer*> mInputToPlayerMap;
};

