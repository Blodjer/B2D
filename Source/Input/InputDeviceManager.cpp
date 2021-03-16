#include "B2D_pch.h"
#include "InputDeviceManager.h"

#include "Game/GameInstance.h"
#include "Game/LocalPlayer.h"

InputDeviceManager::InputDeviceManager(CGameInstance const* const owningGameInstance)
    : m_owningGameInstance(owningGameInstance)
{
#ifdef B2D_PLATFORM_WINDOWS // TODO: replace with desktop platform
    AddInputDevice(InputDeviceID::KeyboardAndMouse);
#endif
}

void InputDeviceManager::AssignNextAvailableDevice(LocalPlayer* localPlayer)
{
    if (m_availableDevices.size() == 0)
    {
        B2D_LOG_INFO("Not enough available input devices. Added local player ({0}) to input device queue.");
        m_pendingPlayers.emplace(localPlayer->GetId());
    }

    for (InputDeviceID inputDeviceId : m_availableDevices)
    {
        localPlayer->AssignInput(inputDeviceId);
        m_assignedDevices.emplace(inputDeviceId, localPlayer->GetId());

        if (inputDeviceId == InputDeviceID::KeyboardAndMouse)
        {
            m_pendingPlayers.emplace(localPlayer->GetId());
        }
    }
}

void InputDeviceManager::DeassignDevices(LocalPlayer* localPlayer)
{
    std::unordered_set<InputDeviceID> const& inputDevices = localPlayer->GetAssignedInputDevices();
    for (InputDeviceID const& inputDevice : inputDevices)
    {
        localPlayer->DeassignInput(inputDevice);
        m_assignedDevices.erase(inputDevice);
    }
}

void InputDeviceManager::AddInputDevice(InputDeviceID inputDeviceId)
{
    // If the device was already assigned
    if (m_assignedDevices.find(inputDeviceId) != m_assignedDevices.end())
    {
        return;
    }

    while (m_pendingPlayers.size() > 0)
    {
        uint32 playerId = m_pendingPlayers.front();
        m_pendingPlayers.pop();

        LocalPlayer* localPlayer = m_owningGameInstance->GetLocalPlayer(playerId);
        if (localPlayer == nullptr)
        {
            continue;
        }

        localPlayer->AssignInput(inputDeviceId);
        m_assignedDevices.emplace(inputDeviceId, playerId);
        return;
    }

    m_availableDevices.insert(inputDeviceId);
}

void InputDeviceManager::RemoveInputDevice(InputDeviceID inputId)
{
    m_availableDevices.erase(inputId);
}
