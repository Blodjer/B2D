#include "B2D_pch.h"
#include "Input.h"

#include "Graphics/Shader.h"
#include "Graphics/Window.h"
#include "Core/GameEngine.h"

InputState Input::mInputState;

void Input::Flush()
{
    mInputState.Flush();
}

bool Input::IsKey(EKey key, EKeyEvent keyEvent)
{
    return mInputState.IsKey(key, keyEvent);
}

void Input::OnKey(EKey key, EKeyEvent keyEvent)
{
    //mInputState.GetKeyState(key) = keyEvent;
}

