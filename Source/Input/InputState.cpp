#include "B2D_pch.h"
#include "InputState.h"

InputState::InputState()
{
    Flush();
}

InputState::InputState(InputState& inputState)
{
    mKeyMap = inputState.mKeyMap;
}

bool InputState::IsKey(EKey key, EKeyEvent keyEvent)
{
    return GetKeyState(key) == keyEvent;
}

EKeyEvent const& InputState::GetKeyState(EKey key)
{
    return mKeyMap.at(static_cast<size_t>(key));
}

void InputState::Flush()
{
    for (EKeyEvent& keyEvent : mKeyMap)
    {
        keyEvent = EKeyEvent::NONE;
    }
}
