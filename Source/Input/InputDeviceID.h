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

    InputDeviceID() : m_inputId(Unknown) {}
    InputDeviceID(uint8 deviceId) : m_inputId(deviceId) {}

    bool InputDeviceID::operator==(InputDeviceID const& other) const
    {
        return m_inputId == other.m_inputId;
    }

private:
    uint8 m_inputId;
};

namespace std
{
    template<>
    struct hash<InputDeviceID>
    {
        size_t operator()(InputDeviceID const& other) const
        {
            return hash<uint8>()(other.m_inputId);
        }
    };
}
