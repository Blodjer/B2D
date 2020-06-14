#pragma once

#include "Input/InputDeviceID.h"

class CGameInstance;
class LocalPlayer;

class InputDeviceManager
{
public:
    InputDeviceManager(CGameInstance const* const owningGameInstance);
    ~InputDeviceManager() = default;

    //SetRumble(inputDeviceId, value)
    //SetLightColor(inputDeviceId, value)

    void AssignNextAvailableDevice(LocalPlayer* localPlayer);
    void DeassignDevices(LocalPlayer* localPlayer);

private:
    void AddInputDevice(InputDeviceID inputId);
    void RemoveInputDevice(InputDeviceID inputId);

private:
    CGameInstance const* const m_owningGameInstance = nullptr;

    std::unordered_set<InputDeviceID> m_availableDevices;
    std::unordered_map<InputDeviceID, uint32> m_assignedDevices;
    std::queue<uint32> m_pendingPlayers;
};

// enum class EInputDeviceType : uint8
// {
//     KeyboardAndMouse,
//     Gamepad
// };

// enum class EGamepadType : uint8
// {
//     Unkown,
//     XboxController,
//     PsController
// };