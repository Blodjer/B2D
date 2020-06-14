#include "B2D_pch.h"
#include "InputState.h"

InputState::InputState()
{
    Flush();
}

InputState::InputState(InputState& inputState)
{
    m_keyMap = inputState.m_keyMap;
}

bool InputState::IsKey(EKey key, EKeyEvent keyEvent)
{
    return GetKeyState(key) == keyEvent;
}

EKeyEvent const& InputState::GetKeyState(EKey key)
{
    return m_keyMap.at(static_cast<size_t>(key));
}

void InputState::SetKeyState(EKey key, EKeyEvent event)
{
    m_keyMap.at(static_cast<size_t>(key)) = event;
}

void InputState::Flush()
{
    for (EKeyEvent& keyEvent : m_keyMap)
    {
        keyEvent = EKeyEvent::None;
    }
}
