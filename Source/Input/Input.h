#pragma once

#include "Core/Core.h"
#include "InputState.h"

class Input
{
    friend class GameEngine;

public:
    Input() = default;
    ~Input() = default;

    static bool IsKey(EKey key, EKeyEvent keyEvent);

private:
    static void Flush();
    static void OnKey(EKey key, EKeyEvent keyEvent);

private:
    static InputState ms_inputState;

};

