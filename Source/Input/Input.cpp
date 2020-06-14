#include "B2D_pch.h"
#include "Input.h"

#include "GameEngine.h"

InputState Input::ms_inputState;

void Input::Flush()
{
    ms_inputState.Flush();
}

bool Input::IsKey(EKey key, EKeyEvent keyEvent)
{
    return ms_inputState.IsKey(key, keyEvent);
}

void Input::OnKey(EKey key, EKeyEvent keyEvent)
{
    ms_inputState.SetKeyState(key, keyEvent);
}

