#include "B2D_pch.h"
#include "InputDeviceManager.h"

#include "Game/GameInstance.h"
#include "Game/LocalPlayer.h"

InputDeviceManager::InputDeviceManager(CGameInstance const* const owningGameInstance)
    : mOwningGameInstance(owningGameInstance)
{
#ifdef B2D_PLATFORM_WINDOWS // TODO: replace with desktop platform
    AddInputDevice(InputDeviceID::KeyboardAndMouse);
#endif
}

void InputDeviceManager::AssignNextAvailableDevice(LocalPlayer* localPlayer)
{
    if (mAvailableDevices.size() == 0)
    {
        B2D_CORE_INFO("Not enough available input devices. Added local player ({0}) to input device queue.");
        mPendingPlayers.emplace(localPlayer->GetId());
    }

    for (InputDeviceID inputDeviceId : mAvailableDevices)
    {
        localPlayer->AssignInput(inputDeviceId);
        mAssignedDevices.emplace(inputDeviceId, localPlayer->GetId());

        if (inputDeviceId == InputDeviceID::KeyboardAndMouse)
        {
            mPendingPlayers.emplace(localPlayer->GetId());
        }
    }
}

void InputDeviceManager::DeassignDevices(LocalPlayer* localPlayer)
{
    std::unordered_set<InputDeviceID> const& inputDevices = localPlayer->GetAssignedInputDevices();
    for (InputDeviceID const& inputDevice : inputDevices)
    {
        localPlayer->DeassignInput(inputDevice);
        mAssignedDevices.erase(inputDevice);
    }
}

void InputDeviceManager::AddInputDevice(InputDeviceID inputDeviceId)
{
    // If the device was already assigned
    if (mAssignedDevices.find(inputDeviceId) != mAssignedDevices.end())
    {
        return;
    }

    while (mPendingPlayers.size() > 0)
    {
        uint32 playerId = mPendingPlayers.front();
        mPendingPlayers.pop();

        LocalPlayer* localPlayer = mOwningGameInstance->GetLocalPlayer(playerId);
        if (localPlayer == nullptr)
        {
            continue;
        }

        localPlayer->AssignInput(inputDeviceId);
        mAssignedDevices.emplace(inputDeviceId, playerId);
        return;
    }

    mAvailableDevices.insert(inputDeviceId);
}

void InputDeviceManager::RemoveInputDevice(InputDeviceID inputId)
{
    mAvailableDevices.erase(inputId);
}
