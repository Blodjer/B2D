#pragma once

#include "Core/Core.h"
#include "Input/InputState.h"

class LocalPlayer
{
public:
    LocalPlayer(uint32 id);
	~LocalPlayer() = default;

    void AssignInput(InputDeviceID inputDeviceId);
    void DeassignInput(InputDeviceID inputDeviceId);

	uint32 GetId() const { return m_id; };
    std::unordered_set<InputDeviceID> const& GetAssignedInputDevices() const { return m_assignedInput; }

private:
    uint32 const m_id;

    std::unordered_set<InputDeviceID> m_assignedInput;
    InputState m_inputState;

};
