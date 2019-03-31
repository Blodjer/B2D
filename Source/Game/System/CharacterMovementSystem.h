#pragma once
#include "Game/Core/System.h"

class CharacterMovementSystem : public System
{
    DECLARE_SYSTEM(CharacterMovementSystem);

public:
    virtual void Update(float deltaTime) override;
};

