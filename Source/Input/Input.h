#pragma once

#include "Core/Core.h"
#include "InputState.h"

class Input
{
    friend class CGameEngine;
    friend class CWindow;

public:
    Input() = default;
    ~Input() = default;

    static bool IsKey(EKey key, EKeyEvent keyEvent);
    //static InputState const& GetGlobalInputState();

private:
    //static InputState& GetGlobalInputStateWritable();
    static void Flush();
    static void OnKey(EKey key, EKeyEvent keyEvent);

private:
    static InputState mInputState;

};

