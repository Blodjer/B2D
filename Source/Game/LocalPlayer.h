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

	uint32 GetId() const { return mId; };
    std::unordered_set<InputDeviceID> const& GetAssignedInputDevices() const { return mAssignedInput; }

private:
    uint32 const mId;

    std::unordered_set<InputDeviceID> mAssignedInput;
    InputState mInputState;

};
