#pragma once

#include "Core/Core.h"

class InputDeviceID
{
    friend struct std::hash<InputDeviceID>;

public:
    enum : uint8
    {
        Unknown,
        KeyboardAndMouse,
        Gamepad_0,
        Gamepad_1,
        Gamepad_2,
        Gamepad_3,
        Gamepad_4,
        Gamepad_5,
        Gamepad_6,
        Gamepad_7,
        Gamepad_8
    };

    InputDeviceID() : mInputId(Unknown) {}
    InputDeviceID(uint8 deviceId) : mInputId(deviceId) {}

    bool InputDeviceID::operator==(InputDeviceID const& other) const
    {
        return mInputId == other.mInputId;
    }

private:
    uint8 mInputId;
};

namespace std
{
    template<>
    struct hash<InputDeviceID>
    {
        size_t operator()(InputDeviceID const& other) const
        {
            return hash<uint8>()(other.mInputId);
        }
    };
}
