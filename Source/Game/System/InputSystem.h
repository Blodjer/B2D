#pragma once
#include "Game/Core/System.h"

class InputSystem : public System
{
    DECLARE_SYSTEM(InputSystem);

public:
    virtual void Update(float deltaTime) override;
};

