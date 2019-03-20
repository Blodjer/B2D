#pragma once
#include "System.h"

class InputSystem : public System
{
    DECLARE_SYSTEM(InputSystem);

public:
    InputSystem();

    virtual void Update(float deltaTime) override;
};

