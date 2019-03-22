#pragma once
#include "System.h"

class CharacterMovementSystem : public System
{
    DECLARE_SYSTEM(CharacterMovementSystem);

public:
    virtual void Update(float deltaTime) override;
};

