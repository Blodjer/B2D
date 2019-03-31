#include "B2D_pch.h"
#include "Input.h"

#include "GameEngine.h"

InputState Input::sInputState;

void Input::Flush()
{
    sInputState.Flush();
}

bool Input::IsKey(EKey key, EKeyEvent keyEvent)
{
    return sInputState.IsKey(key, keyEvent);
}

void Input::OnKey(EKey key, EKeyEvent keyEvent)
{
    sInputState.SetKeyState(key, keyEvent);
}

